#pragma once 

#include "Connectivity.h"
#include "../Particles/Particle.h"

namespace SAPHRON
{
	// Class representing P2 Legendre polynomial (uniaxial) spin anchoring on a particle. 
	// The Hamiltonian defines a penalty function -coeff*P2(dot(d,n)) where d is the director of 
	// the particle and n is the anchoring director.
	class P2SAConnectivity : public Connectivity
	{
		private:
			double _coeff;
			Director _dir;
		public:
			// Creates a new spin anchoring connectivity with an anchoring coefficient 
			// of "coeff" and an anchoring direction of "dir".
			P2SAConnectivity(double coeff, Director dir) : _coeff(coeff), _dir(dir){}
			
			// Evaluate Hamiltonian.
			virtual double EvaluateEnergy(const Particle& p) override
			{
				auto& dir = p.GetDirector();
				double dot = arma::dot(_dir, dir);
				return -1*_coeff*(dot*dot - 0.5);
			}
			
	};
}