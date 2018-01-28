#pragma once 

#include "Constraint.h"
#include "../Particles/Particle.h"
#include "../Worlds/World.h"
#include <armadillo>

namespace SAPHRON
{
	// Class for applying periodic potential
	class HedgehogC : public Constraint, public ParticleObserver
	{
	private:
		// Penalty coefficient.
		double _coeff; 

		// Center of circle
		double _cx, _cy;
		    
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

		bool IsAtBoundary(const Position& pos)
		{
		    double r = sqrt((pos[0] - _cx)*(pos[0] - _cx) + (pos[1] - _cy)*(pos[1] - _cy));
		    double dr = 0.5;
		    double r0 = 6;
		    if( (r>r0-dr) && (r<r0+dr) )
			//if( r>r0 )
			return true;
		    else
			return false;
		}

		double BoundaryEnergy(const Position& pos, const Director& dir)
		{
		    Director radial_vec;
		    radial_vec[0] = pos[0] - _cx;
		    radial_vec[1] = pos[1] - _cy;
		    radial_vec = radial_vec/fnorm(radial_vec);
		    return fdot(radial_vec, dir)*fdot(radial_vec, dir);
		}
		
		double BoundaryEnergy2(const Position& pos, const Director& dir)
		{
		    Director radial_vec, tangent_vec;
		    radial_vec[0] = pos[0] - _cx;
		    radial_vec[1] = pos[1] - _cy;
		    radial_vec = radial_vec/fnorm(radial_vec);
		    tangent_vec[0] = -radial_vec[1];
		    tangent_vec[1] = radial_vec[0];
		    return fdot(tangent_vec, dir)*fdot(tangent_vec, dir);
		}
		
	public:
		// Creates a periodic potential in a region.
		// world - pointer to world.
		// coeff - magnitude of bias. 
		// dir   - constraint direction.
		// index - dimension (x = 0, y = 1, z = 2).
		// lim   - Limits of constraint along specified dimension.
		HedgehogC(
			World* world, 
			double coeff, 
			Director dir, 
			int index, 
			const std::array<double, 2> lim) : 
		Constraint(world), _coeff(coeff), _dir(dir), _index(index), 
		_lim(lim), _pcount(0), _potential(0)
		{
			using namespace arma;
			auto H = world->GetHMatrix();
			_cx = .5*(H(0,0) - 0.0);
			_cy = .5*(H(1,1) - 0.0);

			printf("_cx = %f, _cy = %f\n", _cx, _cy);
			// Go through world particles and observe.
			for(auto& p : *world)
			{
				p->AddObserver(this);
				if(IsAtBoundary(p->GetPosition()))
				{
				    //_potential += BoundaryEnergy(p->GetPosition(), p->GetDirector());
				    _potential += BoundaryEnergy2(p->GetPosition(), p->GetDirector());
				}
			}
		}

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
			if(pEvent.director && IsAtBoundary(pos))
			{
				auto& pdir = pEvent.GetOldDirector();
				//_potential += BoundaryEnergy(pos, dir) -
				//    BoundaryEnergy(pos, pdir);
				_potential += BoundaryEnergy2(pos, dir) -
				    BoundaryEnergy2(pos, pdir);
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
				if(!IsAtBoundary(ppos) && IsAtBoundary(pos))
				{
				    //_potential += BoundaryEnergy(pos, dir);
				    _potential += BoundaryEnergy2(pos, dir);
				}
				else if(IsAtBoundary(ppos) && !IsAtBoundary(pos))
				{
				    //_potential += BoundaryEnergy(pos, dir);
				    _potential += BoundaryEnergy2(pos, dir);
				}
			}			
		}

		void UpdateConstraint(const int iter)
		{			
			/* if ( iter % 1000 == 0 ) */
			/* { */
			/* 	printf("UpdateConstraint at iter = %d", iter); */
			/* 	Director newdir; */
			/* 	newdir[0] = -_dir[1]; */
			/* 	newdir[1] = _dir[0]; */
			/* 	_dir[0] = newdir[0]; */
			/* 	_dir[1] = newdir[1]; */
			/* } */
		}


		void ConstraintMove(World& world)
		{
			printf("ConstraintMove for Hedgehog. \n");
			printf("Do nothing here. \n");
		}		

		void Serialize(Json::Value& json) const override
		{
			json["type"] = "Hedgehog";
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
