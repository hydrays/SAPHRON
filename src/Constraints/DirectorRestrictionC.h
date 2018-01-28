#pragma once 

#include "Constraint.h"
#include "../Particles/Particle.h"
#include "../Worlds/World.h"
#include <armadillo>

namespace SAPHRON
{
	// Class for applying P2 Legendre polynomial constraint on a particle  
	// directors in a 2d region of space. The penalty function is 
	// V = -c*P2(dot(d,n)) where d is the regional director and n is the 
	// constraint direction. Based on location in either x, y, or z plane the 
	// constraint will be applied.
	class DirectorRestrictionC : public Constraint, public ParticleObserver
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

		// Q tensor and associated eigvec/vals.
		Matrix2D _Q;
		arma::cx_colvec2 _eigval;
		arma::cx_mat22 _eigvec; 
		arma::uword _imax; 

		double _penalty;

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

		// Update Q tensor by performing eigen decomposition.
		void UpdateQTensor()
		{
			if(!arma::eig_gen(_eigval, _eigvec, _Q))
			   std::cerr << "Eigenvalue decomposition failed!!" << std::endl;

			_eigval.max(_imax);
		}

	public:
		// Creates a dynamic P2 constraint particle directors in a region.
		// world - pointer to world.
		// coeff - magnitude of bias. 
		// dir   - constraint direction.
		// index - dimension (x = 0, y = 1, z = 2).
		// lim   - Limits of constraint along specified dimension.
		DirectorRestrictionC(
			World* world, 
			double coeff, 
			Director dir, 
			int index, 
			const std::array<double, 2> lim) : 
		Constraint(world), _coeff(coeff), _dir(dir), _index(index), 
		_lim(lim), _Q(arma::fill::zeros), _eigval(arma::fill::zeros), 
		_eigvec(arma::fill::zeros), _imax(0), _pcount(0), _penalty(0)
		{
			using namespace arma;

			// Go through world particles and observe.
			for(auto& p : *world)
			{
				p->AddObserver(this);
				if(IsInRegion(p->GetPosition()))
				{
					++_pcount;
					auto& u = p->GetDirector();
					_Q += kron(u.t(), u) - 1.0/2.0*eye(2,2);
					_penalty += fdot(u, _dir)*fdot(u, _dir);
				}
			}

			// Average
			//_Q *= 2.0/(2.0*_pcount);
			_penalty *= 1.0/_pcount;
			//UpdateQTensor();
		}

		double EvaluateEnergy() const override 
		{
		// 	double dot = (
		// 		_eigvec(0, _imax).real()*_dir[0] + 
		// 		_eigvec(1, _imax).real()*_dir[1]
		// 	);

			// printf("inside DirectorRestrictionC->EvaluateEnergy\n");
			// printf("target [%f, %f] \n", _dir[0], _dir[1]);
			// printf("current [%f, %f] \n", _eigvec(0, _imax).real(), _eigvec(1, _imax).real());
			// printf("eigenvalue [%f, %f] \n", _eigval(_imax).real(), _eigval(0).real());
			//return -1.0*_coeff*(dot*dot - 0.5);
			// return -1.0*_coeff*(2.0*dot*dot);
			return -1.0*_coeff*_penalty;
		}

		// Update Q tensor on particle director change.
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
				//_Q += 2.0/(2.0*_pcount)*(kron(dir.t(), dir) - kron(pdir.t(), pdir));
				//UpdateQTensor();
				//_penalty += (1.0/_pcount)*(fdot(dir,_dir)*fdot(dir,_dir) - fdot(pdir,_dir)*fdot(pdir,_dir));
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
					// Update normalization.
					// _Q *= _pcount/(_pcount + 1.);
					_penalty *= _pcount/(_pcount + 1.);
					++_pcount;

					// _Q += 2.0/(2.0*_pcount)*(kron(dir.t(), dir) - 1.0/2.0*eye(2,2));
					// UpdateQTensor();
					_penalty *= _pcount/(_pcount + 1.);
					_penalty += (1.0/_pcount)*(fdot(dir,_dir)*fdot(dir,_dir));
				}
				else if(IsInRegion(ppos) && !IsInRegion(pos))
				{
					_Q *= _pcount/(_pcount - 1.);
					_penalty *= _pcount/(_pcount - 1.);
					--_pcount;

					_Q -= 2.0/(2.0*_pcount)*(kron(dir.t(), dir) - 1.0/2.0*eye(2,2));
					UpdateQTensor();
					_penalty -= (1.0/_pcount)*(fdot(dir,_dir)*fdot(dir,_dir));
				}
			}			
		}

		void UpdateConstraint(const int iter)
		{

		}

		void ConstraintMove(World& world)
		{
			printf("ConstraintMove for DirectRestriction. \n");
			printf("Do nothing here. \n");
		}	

		void Serialize(Json::Value& json) const override
		{
			json["type"] = "DirectorRestriction";
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