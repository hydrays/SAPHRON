#pragma once

#include "Move.h"
#include "../Utils/Rand.h"
#include "../Utils/Helpers.h"
#include "../Worlds/WorldManager.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../DensityOfStates/DOSOrderParameter.h"

namespace SAPHRON
{
	// Class for performing particle rotations. Method is taken 
	// from Allen and Tildesley based on reference below:
	// J.A. Barker, R.O. Watts, Chem. Phys. Lett., March 1969, 144-145.
	class RotateMove : public Move
	{
	private:
		// Maximum angle displacement [0,2*pi].
		double _maxangle;
		Rand _rand;
		int _rejected;
		int _performed;
		unsigned _seed;

	public:
		RotateMove(double maxangle, unsigned seed = 98476) : 
		_maxangle(maxangle), _rand(seed), 
		_rejected(0), _performed(0), _seed(seed)
		{
		}

		// Rotate a particle and children given a rotation matrix R.
		void Rotate(Particle* particle, const Matrix2D& R)
		{
			// First rotate particle director.
			particle->SetDirector(R*particle->GetDirector());

			// Get COM.
			auto& com = particle->GetPosition();

			// Rotate particle children if it has any 
			for(auto& child : *particle)
			{
				child->SetPosition(R*(child->GetPosition()-com) + com);
				child->SetDirector(R*child->GetDirector());
			}
		}

		// Roate a particle about an axis (x = 1, y = 2, z = 3) 
		// "deg" degrees
		void Rotate(Particle* particle, double deg)
		{
			Matrix2D R = GenRotationMatrix(deg);
			Rotate(particle, R);
		}

		// Perform rotation on a random particle from a random world.
		virtual void Perform(WorldManager* wm, 
							 ForceFieldManager* ffm, 
							 const MoveOverride& override) override
		{
			// Get random particle from random world.
			World* w = wm->GetRandomWorld();
			Particle* particle = w->DrawRandomParticle();

			// Evaluate initial particle energy. 
			auto ei = ffm->EvaluateInterEnergy(*particle);
			ei.energy.constraint = w->GetEnergy().constraint;

			// Choose random axis, and generate random angle.
			double deg = (2.0*_rand.doub() - 1.0)*_maxangle;
			Matrix2D R = GenRotationMatrix(deg);

			// Rotate particle.
			Rotate(particle, R);
			++_performed;

			// Update neighbor list if needed.
			w->CheckNeighborListUpdate(particle->GetChildren());

			// Evaluate final particle energy and get delta E. 
			auto ef = ffm->EvaluateInterEnergy(*particle);
			ef.energy.constraint = ffm->EvaluateConstraintEnergy(*w);

			Energy de = ef.energy - ei.energy;

			// Get sim info for kB.
			auto sim = SimInfo::Instance();

			// Acceptance probability.
			double p = exp(-de.total()/(w->GetTemperature()*sim.GetkB()));
			p = p > 1.0 ? 1.0 : p;

			// printf("inside RotateMove->perfom: angle = %f, de = %f, p = %f\n", deg, de.total(), p);
			// printf("e.con1 = %f, e.con2 = %f\n", ei.energy.constraint, ef.energy.constraint);
			// Reject or accept move.
			if(!(override == ForceAccept) && (p < _rand.doub() || override == ForceReject))
			{
				// Rotate back. 
				Rotate(particle, R.t());
	
				// Update neighbor list if needed.
				w->CheckNeighborListUpdate(particle->GetChildren());

				++_rejected;
			}
			else
			{
				// Update energies and pressures.
				w->IncrementEnergy(de);
				w->IncrementPressure(ef.pressure - ei.pressure);
			}
		}

		// DOS interface for move.
		virtual void Perform(World* w, 
							 ForceFieldManager* ffm, 
							 DOSOrderParameter* op , 
							 const MoveOverride& override) override
		{
			Particle* particle = w->DrawRandomParticle();

			// Evaluate initial particle energy. 
			auto ei = ffm->EvaluateInterEnergy(*particle);
			ei.energy.constraint = ffm->EvaluateConstraintEnergy(*w);
			auto opi = op->EvaluateOrderParameter(*w);

			// Choose random axis, and generate random angle.
			double deg = (2.0*_rand.doub() - 1.0)*_maxangle;
			Matrix2D R = GenRotationMatrix(deg);

			// Rotate particle.
			Rotate(particle, R);
			++_performed;

			// Update neighbor list if needed.
			w->CheckNeighborListUpdate(particle->GetChildren());

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
				Rotate(particle, R.t());
				
				// Update energies and pressures.
				w->IncrementEnergy(-1.0*de);
				w->IncrementPressure(ei.pressure - ef.pressure);
		
				// Update neighbor list if needed.
				w->CheckNeighborListUpdate(particle->GetChildren());
	
				++_rejected;
			}	

		}

		double GetMaxAngle() const { return _maxangle; }

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
			json["type"] = "Rotate";
			json["seed"] = _seed;
			json["maxangle"] = _maxangle;
		}

		virtual std::string GetName() const override { return "Rotate"; }

		// Clone move.
		Move* Clone() const override
		{
			return new RotateMove(static_cast<const RotateMove&>(*this));
		}

		// Perform rotation on a random particle from a random world.
		virtual void Integrator(WorldManager* wm, 
							 ForceFieldManager* ffm)
		{

			double dt = 1e-2;
			// Get random particle from random world.
			World* world = wm->GetWorld(0);

			// moveA();
			for(auto& particle : *world)
			{
				auto& dir = particle->GetDirector();
				auto deg_drift = dt*ffm->EvaluateTorque(*particle);
				double deg_noise = sqrt(2.0*dt*world->GetTemperature())*_rand.gauss();
				//printf("%f \n", _rand.gauss());
				//printf("inside RotateMove->Integrator: deg_drift = %f, deg_noise = %f, gauss = %f\n",
				//       deg_drift, deg_noise, _rand.gauss());
				Matrix2D R = GenRotationMatrix(deg_drift + deg_noise);
				Rotate(particle, R);
			}
    		// calcForce();
			// ffm.calcForce();
    		// moveB();
		}
	};
}
