#pragma once 

#include <functional>
#include <armadillo>
#include <algorithm>
#include <map>
#include "Connectivity.h"
#include "../Particles/Particle.h"
#include "../Particles/ParticleObserver.h"
#include "../Worlds/World.h"

namespace SAPHRON
{
	typedef std::function<void(const Particle&, Director&)> DirectorFunc;
	typedef std::function<int(const Particle&)> PFilterFunc;
	// Class representing De-Localized Spin Anchoring (DLSA) on a particle.
	// The de-localization is w.r.t. a user-supplied filter that uses a 
	// de-localized director to compute the anchoring potential rather than 
	// the particle potential itself. This allows for more particle mobility.
	// The Hamiltonian H = -coeff*P2(dot((n,d)) where n is the de-localized director 
	// and d is the director from a user-supplied function.
	class DLSAConnectivity : public Connectivity, public ParticleObserver
	{
		private:
			std::map<int, int> _groupMap;
			std::map<int, arma::vec> _idmap;
			std::vector<arma::mat> _Qmats;
			Director _tmpVec;
			std::vector<int> _groupCounts;
			arma::cx_vec _eigval;
			arma::cx_mat _eigvec;
			arma::uword _imax;
			DirectorFunc _dfunc;
			Director _dir;
			double _coeff;
			PFilterFunc _pfunc;

		public:
			DLSAConnectivity(const World& world, double coeff, DirectorFunc dfunc, PFilterFunc pfunc) :
				_groupMap(), _idmap(), _Qmats(0), _tmpVec(arma::fill::zeros), _groupCounts(0), 
				_eigval(2, arma::fill::zeros), _eigvec(2,2,arma::fill::zeros), _imax(0), _dfunc(dfunc), 
				_dir({ 0.0, 0.0 }), _coeff(coeff),  _pfunc(pfunc)
			{
				std::vector<int> _groupVec(0);

				for (int i = 0; i < world.GetParticleCount(); ++i)
				{
					auto particle = world.SelectParticle(i);
					int id = particle->GetGlobalIdentifier();
					int group = _pfunc(*particle);

					// See if the group is already registered in the vector. If not add it.
					auto loc = std::find(std::begin(_groupVec),std::end(_groupVec), group);
					if(loc == std::end(_groupVec))
					{
						_groupVec.push_back(group);
						_Qmats.push_back(arma::mat(2,2, arma::fill::zeros));
						_groupCounts.push_back(0);
						loc = std::find(std::begin(_groupVec),std::end(_groupVec), group);
					}

					// Get director and build up Q matrix.
					int index = loc - _groupVec.begin();
					auto& dir = particle->GetDirector();
					_tmpVec = dir;

					_Qmats[index] += arma::kron(_tmpVec.t(), _tmpVec) - 1.0/2.0*arma::eye(2,2);
					_groupCounts[index]++;
					_groupMap.insert(std::pair<int,int>(id, index));
					_idmap.insert(std::pair<int, arma::vec>(id, _tmpVec));
				}

				// Average
				for (int i = 0; i < (int)_Qmats.size(); ++i)
					_Qmats[i] *= 1.0/_groupCounts[i];			
			}

			// Evaluate Hamiltonian.
			inline virtual double EvaluateEnergy(const Particle& p) override
			{
				int id = p.GetGlobalIdentifier();
				auto loc = _groupMap.find(id);
				if(loc == _groupMap.end())
					return 0.0;

				// Calculate eigenpairs.
				int index = loc->second;
				if(!arma::eig_gen(_eigval, _eigvec, _Qmats[index]))
				   std::cout << "Failed!!" << std::endl;

				_eigval.max(_imax);

				// Calculate director based on user supplied func.
				_dfunc(p, _dir);
				
				double dot = arma::dot(arma::real(_eigvec.col(_imax)), _dir);	
;
				return -1.0*_coeff*(1.5*dot*dot-0.5);
			}

			// Update delocalized director on particle director change.
			virtual void ParticleUpdate(const ParticleEvent& pEvent) override
			{
				if (!pEvent.director)
					return;

				auto* p = pEvent.GetParticle();
				int id = p->GetGlobalIdentifier();
				auto loc = _groupMap.find(id);
				if (loc == _groupMap.end())
					return;

				// Update Q.
				int index = loc->second;
				auto& dir = p->GetDirector();
				arma::vec& prevDir = _idmap[id];

				_tmpVec = dir;
				
				_Qmats[index] += 2.0 / (2.0*_groupCounts[index])*(arma::kron(_tmpVec.t(), _tmpVec) - arma::kron(prevDir.t(), prevDir));
				prevDir = _tmpVec;
			}
	};
}
