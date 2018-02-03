#pragma once 

#include "Constraint.h"
#include "../Particles/Particle.h"
#include "../Worlds/World.h"
#include <armadillo>

namespace SAPHRON
{
	// Class for applying periodic potential
	class PlanarSquareC : public Constraint, public ParticleObserver
	{
	private:
		// Penalty coefficient.
		double _coeff; 

		// Center of circle
		double _xmax, _ymax;
		    
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

		double BoundaryEnergy(const Position& pos, const Director& dir)
		{
		    double e;
		    Director vec;
		    if(pos[0] < _lim[0] || pos[0] > _lim[1])
		    {
			vec[0] = 0.0;
			vec[1] = 1.0;
			e = fdot(vec, dir)*fdot(vec, dir);
		    }
		    else if(pos[1] < _lim[0] || pos[1] > _lim[1])
		    {
			vec[0] = 1.0;
			vec[1] = 0.0;
			e = fdot(vec, dir)*fdot(vec, dir);
		    }
		    else
		    {
			e = 0.0;
		    }
		    return e;
		}
		
	public:
		// Creates a periodic potential in a region.
		// world - pointer to world.
		// coeff - magnitude of bias. 
		// dir   - constraint direction.
		// index - dimension (x = 0, y = 1, z = 2).
		// lim   - Limits of constraint along specified dimension.
	PlanarSquareC(
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
			_xmax = H(0,0);
			_ymax = H(1,1);

			printf("_xmax = %f, _ymax = %f\n", _xmax, _ymax);
			// Go through world particles and observe.
			for(auto& p : *world)
			{
				p->AddObserver(this);
				_potential += BoundaryEnergy(p->GetPosition(), p->GetDirector());
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
			if(pEvent.director)
			{
				auto& pdir = pEvent.GetOldDirector();
				_potential += BoundaryEnergy(pos, dir) -
				    BoundaryEnergy(pos, pdir);
				return;
			}
			else if(pEvent.position)
			{
			    // do nothing
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
			//printf("ConstraintMove for PlanarSquare. \n");
			double dt = 1e-3;
			Director vec;
			double dr = 0.0;
			int change_flag;
			// moveA();
			for(auto& particle : world)
			{
				auto& dir = particle->GetDirector();
				auto& pos = particle->GetPosition();
		    
			    if(pos[0] < _lim[0] || pos[0] > _lim[1])
			    {
				vec[0] = 0.0;
				vec[1] = 1.0;
				change_flag = 1;
			    }
			    else if(pos[1] < _lim[0] || pos[1] > _lim[1])
			    {
				vec[0] = 1.0;
				vec[1] = 0.0;
				change_flag = 1;
			    }
			    else
			    {
				change_flag = 0;
			    }

			 //    if( (pos[0] > _lim[0]+2.0*dr) && 
			 //    	(pos[0] < _lim[1]-2.0*dr) && 
			 //    	(fabs(pos[1]-_lim[0])<dr || fabs(pos[1]-_lim[1])<dr) )
			 //    {
				// vec[0] = 1.0;
				// vec[1] = 0.0;
				// change_flag = 1;
			 //    }
			 //    else if( (pos[1] > _lim[0]+2.0*dr) && 
			 //    	(pos[1] < _lim[1]-2.0*dr) && 
			 //    	(fabs(pos[0]-_lim[0])<dr || fabs(pos[0]-_lim[1])<dr) )
			 //    {
				// vec[0] = 0.0;
				// vec[1] = 1.0;
			 //    change_flag = 1;
				// }
				// else
				// {
				// 	change_flag = 0;
				// }

				if (change_flag==1)
				{
				auto deg = _coeff*fdot(dir,vec)*(dir[0]*vec[1] - dir[1]*vec[0])*dt;
				if (deg > 0.2)
					deg = 0.2;
				else if (deg < -0.2)
					deg = -0.2;
				else
				{
					
				}
				Matrix2D R = GenRotationMatrix(deg);

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
			}
		}	

		void Serialize(Json::Value& json) const override
		{
			json["type"] = "PlanarSquare";
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
