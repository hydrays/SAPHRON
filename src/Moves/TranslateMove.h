#pragma once 

#include "../Utils/Rand.h"
#include "Move.h"
#include "../Worlds/WorldManager.h"
#include "../Simulation/SimInfo.h"
#include "../DensityOfStates/DOSOrderParameter.h"
#include "../ForceFields/ForceFieldManager.h"

namespace SAPHRON
{
	// Class for translating a random particle a maximum of "dx" distance.
	class TranslateMove : public Move
	{
	private: 
		double _dx;
		Rand _rand;
		int _rejected;
		int _performed;
		unsigned _seed;

		// Explicit draw for _sdx? Use for low fraction particles.
		bool _explicit;

		// Vector of dx's based on species ID.
		std::vector<double> _sdx;
		std::vector<int> _species;

	public: 
		// Initialize translate move with species based dx. Anything not 
		// specified will initialize to zero.
		TranslateMove(const std::map<int, double>& dx, bool expl, unsigned seed = 2496) : 
		_dx(0), _rand(seed), _rejected(0), _performed(0), _seed(seed), _explicit(expl), 
		_sdx(0), _species(0)
		{
			// Get species and initialize all dx's with zeros.
			auto& list = Particle::GetSpeciesList();
			_sdx.resize(list.size(), 0);

			for(auto& id : dx)
			{	
				if(id.first >= (int)list.size())
				{
					std::cerr << "Species ID \"" 
							  << id.first << "\" provided does not exist." 
							  << std::endl;
					exit(-1);
				}
				_sdx[id.first] = id.second;
				_species.push_back(id.first);
			}
		}

		// Initialize translate move with species based dx. Anything not specified 
		// will initialize to zero.
		TranslateMove(const std::map<std::string, double>& dx, bool expl, unsigned seed = 2496) : 
		_dx(0), _rand(seed), _rejected(0), _performed(0), _seed(seed), _explicit(expl), 
		_sdx(0), _species(0)
		{
			// Get species and initialize all dx's with zeros.
			auto& list = Particle::GetSpeciesList();
			_sdx.resize(list.size(), 0);
			_species.resize(dx.size());

			// Resolve strings to ID's and fill sdx.
			for(auto& id : dx)
			{
				auto it = std::find(list.begin(), list.end(), id.first);
				if(it == list.end())
				{
					std::cerr << "Species ID \""
							  << id.first << "\" provided does not exist."
							  << std::endl;
					exit(-1);
				}

				_sdx[it - list.begin()] = id.second;
				_species.push_back(it - list.begin());
			}
		}

		TranslateMove(double dx, unsigned seed = 2496) : 
		_dx(dx), _rand(seed), _rejected(0), _performed(0), _seed(seed), _explicit(false), _sdx(0)
		{
		}

		// This is for unit testing convenience. It is not part of the interface.
		void Perform(Particle* particle)
		{
			auto& pos = particle->GetPosition();
			particle->SetPosition(pos[0] + _dx*(_rand.doub()-0.5), 
								  pos[1] + _dx*(_rand.doub()-0.5));
			++_performed;
		}

		// Perform translation on a random particle from a random world.
		virtual void Perform(WorldManager* wm, ForceFieldManager* ffm, const MoveOverride& override) override
		{
			//printf("inside move->perform: 00\n");

			// Get random particle from random world.
			World* w = wm->GetRandomWorld();
			if(w->GetParticleCount() == 0)
				return;
			
			auto dx = _dx;			
			Particle* particle = nullptr;
			
			// If we explicit draw. 
			if(_explicit)
				particle = w->DrawParticleFromSpeciesList(_species);
			else
				particle = w->DrawRandomParticle();

			// If draw is empty, return.
			if(particle == nullptr)
				return;

			// Get dx if necessary.
			// if _dx is not set, then we do species base draw.
			if(_dx == 0)
				dx = _sdx[particle->GetSpeciesID()];			

			// Particle is not translated.
			if(dx == 0)
				return;

			// Initial position.
			auto posi = particle->GetPosition();
			
			// Evaluate initial particle energy. 
			auto ei = ffm->EvaluateInterEnergy(*particle);
			ei.energy.constraint = w->GetEnergy().constraint;

			// Generate new position then apply periodic boundaries.
			Position newPos({posi[0] + dx*(_rand.doub()-0.5), 
							 posi[1] + dx*(_rand.doub()-0.5)});
			
			w->ApplyPeriodicBoundaries(&newPos);
			particle->SetPosition(newPos);
			++_performed;		

			// Update neighbor list if needed.
			w->CheckNeighborListUpdate(particle);						

			//printf("inside move->perform: 01\n");
			// Evaluate final particle energy and get delta E. 
			auto ef = ffm->EvaluateInterEnergy(*particle);
			ef.energy.constraint = ffm->EvaluateConstraintEnergy(*w);
			Energy de = ef.energy - ei.energy;
			
			// Get sim info for kB.
			auto& sim = SimInfo::Instance();

			// Acceptance probability.
			double p = exp(-de.total()/(w->GetTemperature()*sim.GetkB()));
			p = p > 1.0 ? 1.0 : p;

			//printf("inside move->perform: 02\n");

			// Reject or accept move.
			if(!(override == ForceAccept) && (p < _rand.doub() || override == ForceReject))
			{
				particle->SetPosition(posi);

				// Update neighbor list if needed.
				w->CheckNeighborListUpdate(particle);
				++_rejected;
			}
			else
			{
				// Update energies and pressures.
				w->IncrementEnergy(de);
				w->IncrementPressure(ef.pressure - ei.pressure);
			}	
		}
		
		// Perform move using DOS interface.
		virtual void Perform(World* w, ForceFieldManager* ffm, DOSOrderParameter* op , const MoveOverride& override) override
		{
			if(w->GetParticleCount() == 0)
				return;
			
			auto dx = _dx;			
			Particle* particle = nullptr;
						
			// If we explicit draw. 
			if(_explicit)
				particle = w->DrawParticleFromSpeciesList(_species);
			else
				particle = w->DrawRandomParticle();

			// If draw is empty, return.
			if(particle == nullptr)
				return;

			// Get dx if necessary.
			// if _dx is not set, then we do species base draw.
			if(_dx == 0)
				dx = _sdx[particle->GetSpeciesID()];			

			// Particle is not translated.
			if(dx == 0)
				return;

			// Initial position.
			auto posi = particle->GetPosition();
			
			// Evaluate initial particle energy. 
			auto ei = ffm->EvaluateInterEnergy(*particle);
			ei.energy.constraint = ffm->EvaluateConstraintEnergy(*w);
			auto opi = op->EvaluateOrderParameter(*w);

			// Generate new position then apply periodic boundaries.
			Position newPos({posi[0] + dx*(_rand.doub()-0.5), 
							 posi[1] + dx*(_rand.doub()-0.5)});
			
			w->ApplyPeriodicBoundaries(&newPos);
			particle->SetPosition(newPos);
			++_performed;										
			
			// Update neighbor list if needed.
			w->CheckNeighborListUpdate(particle);

			// Evaluate final particle energy and get delta E. 
			auto ef = ffm->EvaluateInterEnergy(*particle);
			ef.energy.constraint = ffm->EvaluateConstraintEnergy(*w);
			Energy de = ef.energy - ei.energy;
			
			// Update energies and pressures.
			w->IncrementEnergy(de);
			w->IncrementPressure(ef.pressure - ei.pressure);

			auto opf = op->EvaluateOrderParameter(*w);

			// Acceptance probability.
			double p = op->AcceptanceProbability(ei.energy, ef.energy, opi, opf, *w);

			// Reject or accept move.
			if(!(override == ForceAccept) && (p < _rand.doub() || override == ForceReject))
			{
				particle->SetPosition(posi);
				
				// Update energies and pressures.
				w->IncrementEnergy(-1.0*de);
				w->IncrementPressure(ei.pressure - ef.pressure);

				// Update neighbor list if needed.
				w->CheckNeighborListUpdate(particle);
			
				++_rejected;
			}	
		}

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
			json["type"] = "Translate";
			json["seed"] = _seed;
			if(_sdx.size() != 0)
			{
				auto& species = Particle::GetSpeciesList();
				for(size_t i = 0; i < species.size(); ++i)
					json["dx"][species[i]] = _sdx[i];

				json["explicit_draw"] = _explicit;
			}
			else
				json["dx"] = _dx;
		}

		virtual std::string GetName() const override { return "Translate"; }

		// Clone move.
		Move* Clone() const override
		{
			return new TranslateMove(static_cast<const TranslateMove&>(*this));
		}
	};
}