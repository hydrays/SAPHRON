#pragma once 

#include "Constraint.h"
#include "../Particles/Particle.h"
#include "../Worlds/World.h"
#include <armadillo>

namespace SAPHRON
{
	// Class for applying periodic potential
	class PeriodicPotentialC : public Constraint, public ParticleObserver
	{
	private:
		// Penalty coefficient.
		double _coeff; 

		// Preferred direction.
		Director _dir;

		// Index of dimension for restriction region.
		int _index;

		// Bounds on restriction region.
		std::array<double, 2> _lim;

		double _potential;

		// Particle count for averaging.
		int _pcount;

		// Is a position inside the restriction region?
		bool IsInRegion(const Position& pos)
		{
			switch(_index)
			{
				case 0:
					if(pos[0] < _lim[0] || pos[0] > _lim[1])
						return false;
					break;
				default:
					if(pos[1] < _lim[0] || pos[1] > _lim[1])
						return false;
					break;
			}

			return true;
		}

	public:
		// Creates a periodic potential in a region.
		// world - pointer to world.
		// coeff - magnitude of bias. 
		// dir   - constraint direction.
		// index - dimension (x = 0, y = 1, z = 2).
		// lim   - Limits of constraint along specified dimension.
		PeriodicPotentialC(
			World* world, 
			double coeff, 
			Director dir, 
			int index, 
			const std::array<double, 2> lim) : 
		Constraint(world), _coeff(coeff), _dir(dir), _index(index), 
		_lim(lim), _pcount(0), _potential(0)
		{
			using namespace arma;

			// Go through world particles and observe.
			for(auto& p : *world)
			{
				p->AddObserver(this);
				if(IsInRegion(p->GetPosition()))
				{
					auto& u = p->GetDirector();
					_potential += fdot(u, _dir)*fdot(u, _dir);
				}
			}
		}

		// void UpdateDirection(Director dir)
		// {
		// 	_dir[0] = dir[0];
		// 	_dir[1] = dir[1];
		// }

		double EvaluateEnergy() const override 
		{
			return -1.0*_coeff*_potential;
		}
		
		void ParticleUpdate(const ParticleEvent& pEvent) override
		{
			using namespace arma;

			// Get particle and positions, directors.
			auto* p = pEvent.GetParticle();
			auto& pos = p->GetPosition();
			auto& dir = p->GetDirector();
			
			// If only director has changed, check if it's in the region
			// and update.
			if(pEvent.director && IsInRegion(pos))
			{
				auto& pdir = pEvent.GetOldDirector();
				_potential += fdot(dir,_dir)*fdot(dir,_dir) - fdot(pdir,_dir)*fdot(pdir,_dir);
				return;
			}
			else if(pEvent.position)
			{
				auto& ppos = pEvent.GetOldPosition();
				
				// Three possible cases on a position change:
				// 1. Particle previously not in region but now in region. 
				// 2. Particle previously in region and still in region 
				//    (we don't do anything since nothing's changed).
				// 3. Particle previously in region but now not in region.
				if(!IsInRegion(ppos) && IsInRegion(pos))
				{
					_potential += fdot(dir,_dir)*fdot(dir,_dir);
				}
				else if(IsInRegion(ppos) && !IsInRegion(pos))
				{
					_potential -= fdot(dir,_dir)*fdot(dir,_dir);
				}
			}			
		}

		void UpdateConstraint(const int iter)
		{			
			if ( iter % 1000 == 0 )
			{
				printf("UpdateConstraint at iter = %d", iter);
				Director newdir;
				newdir[0] = -_dir[1];
				newdir[1] = _dir[0];
				_dir[0] = newdir[0];
				_dir[1] = newdir[1];
			}
		}

		void Serialize(Json::Value& json) const override
		{
			json["type"] = "PeriodicPotential";
			json["coefficient"] = _coeff;
			json["director"][0] = _dir[0];
			json["director"][1] = _dir[1];
			json["director"][2] = 0.0;
			json["index"] = _index;
			json["limits"][0] = _lim[0];
			json["limits"][1] = _lim[1];
		}
	};
}