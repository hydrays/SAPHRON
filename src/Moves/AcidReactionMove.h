#pragma once 

#include "Move.h"
#include "../Utils/Helpers.h"
#include "../Utils/Rand.h"
#include "../Worlds/WorldManager.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../DensityOfStates/DOSOrderParameter.h"

/* Currently move only works with acid where the following is the reaction:
forward HA -> A- + H+
Reverse A- + H+ -> HA
*/

namespace SAPHRON
{
	class AcidReactionMove : public Move
	{
	private:
		std::vector<int> _reactants;
		std::vector<int> _products;
		Rand _rand;

		int _performed;
		int _rejected;
		bool _prefac;
		int _scount;
		double _pKo;
		double _protoncharge;
		int _Ndeprotonated;
		int _seed;

		void InitStashParticles(const WorldManager& wm)
		{
			// Get particle map, find one of the appropriate species 
			// and clone.
			auto& plist = Particle::GetParticleMap();

			for(auto& id : _products)
			{
				// Since the species exists, we assume there must be at least 
				// one find. 
				auto pcand = std::find_if(plist.begin(), plist.end(), 
					[=](const std::pair<int, Particle*>& p)
					{
						return p.second->GetSpeciesID() == id;
					}
				);

				// Stash a characteristic amount of the particles in world.
				for(auto& world : wm)
					world->StashParticle(pcand->second, _scount);
			}
		}

	public:
		AcidReactionMove(const std::vector<std::string>& reactants,
		const std::vector<std::string>& products,
		const WorldManager& wm,
		int stashcount, double pKo, double protoncharge,
		int seed = 7456253) : 
		_reactants(0),_products(0), _rand(seed), _performed(0),
		 _rejected(0), _prefac(true), _scount(stashcount),
		 _pKo(pKo), _protoncharge(protoncharge),
		 _Ndeprotonated(0), _seed(seed)
		{
			// Verify species list and add to local vector.
			auto& list = Particle::GetSpeciesList();

			for(auto& id : reactants)
			{
				auto it = std::find(list.begin(), list.end(), id);
				if(it == list.end())
				{
					std::cerr << "Species ID \""
							  << id << "\" provided does not exist."
							  << std::endl;
					exit(-1);
				}
				_reactants.push_back(it - list.begin());
			}

			for(auto& id : products)
			{
				auto it = std::find(list.begin(), list.end(), id);
				if(it == list.end())
				{
					std::cerr << "Species ID \""
							  << id << "\" provided does not exist."
							  << std::endl;
					exit(-1);
				}
				_products.push_back(it - list.begin());
			}

			InitStashParticles(wm);
		}

		AcidReactionMove(const std::vector<int>& reactants,
		const std::vector<int>& products,
		const WorldManager& wm,
		int stashcount, double pKo, double protoncharge,
		int seed = 7456253) : 
		_reactants(0),_products(0), _rand(seed), _performed(0),
		 _rejected(0), _prefac(true), _scount(stashcount),
		 _pKo(pKo), _protoncharge(protoncharge),
		 _Ndeprotonated(0), _seed(seed)
		{
			// Verify species list and add to local vector.
			auto& list = Particle::GetSpeciesList();
			
			for(auto& id : reactants)
			{
				if(id >= (int)list.size())
				{
					std::cerr << "Species ID \"" 
							  << id << "\" provided does not exist." 
							  << std::endl;
					exit(-1);
				}
				_reactants.push_back(id);
			}
			
			for(auto& id : products)
			{
				if(id >= (int)list.size())
				{
					std::cerr << "Species ID \"" 
							  << id << "\" provided does not exist." 
							  << std::endl;
					exit(-1);
				}
				_products.push_back(id);
			}

			InitStashParticles(wm);
		} 

		virtual void Perform(WorldManager* wm, 
					 ForceFieldManager* ffm, 
					 const MoveOverride& override) override
		{

			// Draw random particle from random world.
			auto* w = wm->GetRandomWorld();
			auto* p = w->DrawRandomPrimitiveBySpecies(_reactants[0]);
			Particle* ph =nullptr;

			int RxnExtent = 1;
			double rxndirection = _rand.doub();
			double OldCharge = p->GetCharge();
			double OldMass = p->GetMass();
			double Korxn = 0;

			//Determine reaction direction.
			if(rxndirection>=0.5 && OldCharge >= 0) //Forward reaction
			{
				RxnExtent = 1;
			}

			else if(rxndirection<0.5 && OldCharge < 0)//Reverse reaction
			{
				RxnExtent = -1;
				ph = w->DrawRandomPrimitiveBySpecies(_products[0]);

				if(ph==nullptr)
				{
					std::cout<<"No products are present in system!"<<std::endl;
					exit(-1);
				}
			}

			else
				return;

			// Evaluate initial energy. 
			auto ei = ffm->EvaluateHamiltonian(*w);

			if(RxnExtent == -1)
			{
				p->SetCharge(0.0);
				p->SetMass(OldMass + ph->GetMass());
				w->RemoveParticle(ph);
			}

			else
			{
				ph = w->UnstashParticle(_products[0]);
				p->SetCharge(-_protoncharge);
				p->SetMass(OldMass - ph->GetMass());

				// Generate a random position and orientation for particle insertion.
				const auto& H = w->GetHMatrix();
				Vector3D pr{_rand.doub(), _rand.doub(), _rand.doub()};
				Vector3D pos = H*pr;
				ph->SetPosition(pos);

				// Choose random axis, and generate random angle.
				int axis = _rand.int32() % 3 + 1;
				double deg = (4.0*_rand.doub() - 2.0)*M_PI;
				Matrix3D R = GenRotationMatrix(axis, deg);

				// Rotate particle and director.
				ph->SetDirector(R*ph->GetDirector());
				for(auto& child : *ph)
				{
					child->SetPosition(R*(child->GetPosition()-pos) + pos);
					child->SetDirector(R*child->GetDirector());
				}

				// Insert particle.
				w->AddParticle(ph);
			}

			auto id = ph->GetSpeciesID();
			auto& comp = w->GetComposition();
			auto V = pow(w->GetVolume(),RxnExtent);
			auto lambda = w->GetWavelength(id);
			auto lamda3 = pow(lambda*lambda*lambda,-1*RxnExtent);
			double lamdaratio = pow(p->GetMass()/OldMass,3.0/2.0);
			double Nratio=0;

			if(RxnExtent==1)
			{
				Nratio = (comp[p->GetSpeciesID()]-_Ndeprotonated)/((_Ndeprotonated+1)*(comp[ph->GetSpeciesID()]+1));
				Korxn = exp(-_pKo);
			}
			else
			{
				Nratio = double(_Ndeprotonated)*comp[ph->GetSpeciesID()]/(comp[p->GetSpeciesID()]-_Ndeprotonated + 1);
				Korxn = exp(-_pKo);
			}

			++_performed;

			auto ef = ffm->EvaluateHamiltonian(*w);

			auto de = ef - ei;
		
			// Get sim info for kB.
			auto& sim = SimInfo::Instance();

			// Acceptance probability.
			double pacc = Nratio*V*lamda3*lamdaratio*Korxn*
			exp((-de.energy.total())/(w->GetTemperature()*sim.GetkB()));
			pacc = pacc > 1.0 ? 1.0 : pacc;

			// Reject or accept move.
			if(!(override == ForceAccept) && (pacc < _rand.doub() || override == ForceReject))
			{
				++_rejected;
				p->SetCharge(OldCharge);
				p->SetMass(OldMass);

				if(RxnExtent == -1) {w->AddParticle(ph);}

				else {w->StashParticle(ph);}

			}
			else
			{
				if(RxnExtent == -1) 
				{
					w->StashParticle(ph);
					_Ndeprotonated-=1;
				}
				else
					_Ndeprotonated+=1;

				w->SetEnergy(ef.energy);
				w->SetPressure(ef.pressure);
			}
		}

		virtual void Perform(World* world, 
							 ForceFieldManager* ffm, 
							 DOSOrderParameter* op, 
							 const MoveOverride& override) override
		{

			// Draw random particle from random world.
			auto* p = world->DrawRandomPrimitiveBySpecies(_reactants[0]);
			Particle* ph =nullptr;

			int RxnExtent = 1;
			double rxndirection = _rand.doub();
			double OldCharge = p->GetCharge();
			double OldMass = p->GetMass();
			double Korxn = 0;

			//Determine reaction direction.
			if(rxndirection>=0.5 && OldCharge >= 0) //Forward reaction
			{
				RxnExtent = 1;
			}

			else if(rxndirection<0.5 && OldCharge < 0)//Reverse reaction
			{
				RxnExtent = -1;
				ph = world->DrawRandomPrimitiveBySpecies(_products[0]);

				if(ph==nullptr)
				{
					std::cout<<"No products are present in system!"<<std::endl;
					exit(-1);
				}
			}

			else
				return;

			// Evaluate initial energy. 
			auto ei = ffm->EvaluateHamiltonian(*world);
			auto opi = op->EvaluateOrderParameter(*world);

			if(RxnExtent == -1)
			{
				p->SetCharge(0.0);
				p->SetMass(OldMass + ph->GetMass());
				world->RemoveParticle(ph);
			}

			else
			{
				ph = world->UnstashParticle(_products[0]);
				p->SetCharge(-_protoncharge);
				p->SetMass(OldMass - ph->GetMass());

				// Generate a random position and orientation for particle insertion.
				const auto& H = world->GetHMatrix();
				Vector3D pr{_rand.doub(), _rand.doub(), _rand.doub()};
				Vector3D pos = H*pr;
				ph->SetPosition(pos);

				// Choose random axis, and generate random angle.
				int axis = _rand.int32() % 3 + 1;
				double deg = (4.0*_rand.doub() - 2.0)*M_PI;
				Matrix3D R = GenRotationMatrix(axis, deg);

				// Rotate particle and director.
				ph->SetDirector(R*ph->GetDirector());
				for(auto& child : *ph)
				{
					child->SetPosition(R*(child->GetPosition()-pos) + pos);
					child->SetDirector(R*child->GetDirector());
				}

				// Insert particle.
				world->AddParticle(ph);
			}

			auto id = ph->GetSpeciesID();
			auto& comp = world->GetComposition();
			auto V = pow(world->GetVolume(),RxnExtent);
			auto lambda = world->GetWavelength(id);
			auto lamda3 = pow(lambda*lambda*lambda,-1*RxnExtent);
			double lamdaratio = pow(p->GetMass()/OldMass,3.0/2.0);
			double Nratio=0;

			if(RxnExtent==1)
			{
				Nratio = (double(comp[p->GetSpeciesID()]) - _Ndeprotonated)/((_Ndeprotonated+1)*(comp[ph->GetSpeciesID()]+1));
				Korxn = exp(-_pKo);
			}
			else
			{
				Nratio = (double(_Ndeprotonated)*comp[ph->GetSpeciesID()])/(comp[p->GetSpeciesID()]-_Ndeprotonated + 1);
				Korxn = exp(-_pKo);
			}

			++_performed;

			auto ef = ffm->EvaluateHamiltonian(*world);

			// Update energies and pressures.
			world->SetEnergy(ef.energy);
			world->SetPressure(ef.pressure);

			auto opf = op->EvaluateOrderParameter(*world);
			
			// Acceptance probability.
			double pacc = op->AcceptanceProbability(ei.energy, ef.energy, opi, opf, *world);
			if(_prefac)
			{
				auto arg = Nratio*V*lamda3*lamdaratio*Korxn;
				pacc *= arg;
			}
			pacc = pacc > 1.0 ? 1.0 : pacc;

			// Reject or accept move.
			if(!(override == ForceAccept) && (pacc < _rand.doub() || override == ForceReject))
			{
				++_rejected;
				p->SetCharge(OldCharge);
				p->SetMass(OldMass);
				world->SetEnergy(ei.energy);
				world->SetPressure(ei.pressure);

				if(RxnExtent == -1) {world->AddParticle(ph);}

				else {world->StashParticle(ph);}

			}
			else
			{
				if(RxnExtent == -1) 
				{
					world->StashParticle(ph);
					_Ndeprotonated-=1;
				}
				else
					_Ndeprotonated+=1;
			}
		}

		// Turns on or off the acceptance rule prefactor for DOS order parameter.
		void SetOrderParameterPrefactor(bool flag) { _prefac = flag; }

		virtual double GetAcceptanceRatio() const override
		{
			return 1.0-(double)_rejected/_performed;
		};

		virtual void ResetAcceptanceRatio() override
		{
			_performed = 0;
			_rejected = 0;
		}

		// Serialize.
		virtual void Serialize(Json::Value& json) const override
		{
			json["type"] = GetName();
			json["seed"] = _seed;
			json["pKo"] = _pKo;
			json["stash_count"] = _scount;
			json["op_prefactor"] = _prefac;

			for(auto& s : _reactants)
				json["reactants"].append(s);

			for(auto& s : _products)
				json["products"].append(s);
		}

		virtual std::string GetName() const override { return "AcidReaction"; }

		// Clone move.
		Move* Clone() const override
		{
			return new AcidReactionMove(static_cast<const AcidReactionMove&>(*this));
		}

	};
}