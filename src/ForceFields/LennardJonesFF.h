#pragma once 

#include "../Worlds/World.h"
#include "ForceField.h"

namespace SAPHRON
{
	// Class for classic 12-6 Lennard Jones potential.
	class LennardJonesFF : public ForceField
	{
	private:
		double _epsilon;
		double _sigmasq;
		double _sigma3;
		CutoffList _rcsq;
		CutoffList _rc;
		std::vector<double> _etail;
		std::vector<double> _ptail;

	public:

		LennardJonesFF(double epsilon, double sigma, const CutoffList& rc) : 
		_epsilon(epsilon), _sigmasq(sigma*sigma), _sigma3(_sigmasq*sigma),
		_rcsq(0), _rc(rc), _etail(0), _ptail(0)
		{ 
			for(auto& r : _rc)
			{
				auto r3 = r*r*r;
				auto r9 = r3*r3*r3;
				_etail.push_back(
					4.0/3.0*_epsilon*_sigma3*
					(1.0/3.0*(_sigma3*_sigma3*_sigma3)/r9-_sigma3/r3)
				); 
				
				_ptail.push_back(
					8.0*_epsilon*_sigma3*(2.0/3.0*(_sigma3*_sigma3*_sigma3)/r9-_sigma3/r3)
				);

				_rcsq.push_back(r*r);
			}
		}

		virtual Interaction Evaluate(const Particle&, 
									 const Particle&, 
									 const Position& rij,
									 unsigned int wid) const override
		{
			Interaction ep;

			auto rsq = fdot(rij, rij);
			if(rsq > _rcsq[wid])
				return ep;

			double sr6 = _sigma3*_sigma3/(rsq*rsq*rsq);
			ep.energy = 4.0*_epsilon*(sr6*sr6-sr6);
			ep.virial = 24.0*_epsilon*(sr6-2.0*sr6*sr6)/rsq;
			
			return ep;
		}

		virtual double EvaluateTorque(const Particle&, 
									 const Particle&, 
									 const Position& rij,
									 unsigned int wid) const override
		{
			return 0.0;
		}

		virtual double EnergyTailCorrection(unsigned int wid) const override
		{
			return _etail[wid];
		}

		virtual double PressureTailCorrection(unsigned int wid) const override 
		{
			return _ptail[wid];
		}

		// Serialize LJ.
		virtual void Serialize(Json::Value& json) const override
		{	
			json["type"] = "LennardJones";
			json["sigma"] = sqrt(_sigmasq);
			json["epsilon"] = _epsilon;
			for(auto& rc : _rc)
				json["rcut"].append(rc);
		}

		double GetEpsilon() const { return _epsilon; }
		double GetSigma() const { return sqrt(_sigmasq); }
	};
}