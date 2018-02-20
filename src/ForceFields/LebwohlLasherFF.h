#pragma once

#include "ForceField.h"

namespace SAPHRON
{
	// Class for Lebwohl-Lasher interaction. 
	class LebwohlLasherFF : public ForceField
	{
	private:
		// Interaction parameter.
		double _eps;

		// Isotropic interaction parameter.
		double _gamma;

	public:
		// Inistantiate Lebwohl-Lasher force field.
		LebwohlLasherFF(double eps, double gamma) : _eps(eps), _gamma(gamma) {}

		inline virtual Interaction Evaluate(const Particle& p1, 
											const Particle& p2, 
											const Position&,
											unsigned int) const override
		{
			auto& n1 = p1.GetDirector();
			auto& n2 = p2.GetDirector();

			double dot = fdot(n1,n2);

			return {-1.0*(_eps*(1.5*dot*dot - 0.5) + _gamma), 0.0};
			//return {-1.0*(_eps*(35*dot*dot - 30*dot*dot + 15/8) + _gamma), 0.0};
		}

		// Serialize LJ.
		virtual void Serialize(Json::Value& json) const override
		{
			json["type"] = "LebwohlLasher";
			json["gamma"] = _gamma;
			json["epsilon"] = _eps;
		}

		virtual double EvaluateTorque(const Particle& p1,
									 const Particle& p2, 
									 const Position&,
									 unsigned int) const override
		{
			double torque = 0.0;
			
			auto& ui = p1.GetDirector();
			auto& uj = p2.GetDirector();

			auto uiuj = fdot(ui, uj);
			auto dUdg = 2.0*uiuj;
			auto uxu = ui[0]*uj[1] - ui[1]*uj[0];
			torque = 1.5*_eps*dUdg * uxu;
			//printf("inside GayBerne->Evaluate: u1 = (%f, %f), u2 = (%f, %f), r = (%f, %f), R=%f, torque=%f\n", 
			//	ui[0], ui[1], uj[0], uj[1], rij[0], rij[1], R, torque);
			//printf("%f, %f, %f, %f, %f, %f, %f, %f\n", 
			//	ui[0], ui[1], uj[0], uj[1], rij[0], rij[1], R, torque);
			return torque;
		}

		double GetEpsilon() { return _eps; }
		double GetGamma() { return _gamma; }
	};
}
