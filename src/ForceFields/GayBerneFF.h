#pragma once 

#include "ForceField.h"
#include <math.h>

namespace SAPHRON
{
	// Classic anisotropic Gay-Berne potential for interaction between
	// two uniaxial aspherical particles. Expressions and notation obtained from:
	// Golubkov & Ren, J. Chem. Phys. 125, 064103 (2006).
	class GayBerneFF : public ForceField
	{
	private:
		// Commonly used parameters.
		double _sig0, _X, _asq, _Xasq, _Xa_sq, _Xsq, _Xpsq, _Xpapsq, _Xpap_sq;

		// Particle diameters and lengths.
		double _di, _dj, _li, _lj;

		// Shape parameters.
		double _mu, _nu;

		// Particle interaction energies.
		// eps0 = scaling, epsE = end-to-end/face-to-face,
		// epsS = side-to-side.
		double _eps0, _epsE, _epsS;

		// Potential softness.
		double _dw;

		// Cutoff radii.
		CutoffList _rc;

	public:
		// Initialize Gay-Berne potential between two uniaxial aspherical particles. 
		// di, dj: particle diameters.
		// li, lj: particle lengths.
		// eps0, epsE, epsS: cross interaction, end-to-end, side-to-side.
		// dw : potential softness.
		// rc: List of cutoff radii for world(s).
		// mu, nu: Shape parameters. Defaults are 2, 1.
		GayBerneFF(
			double di, double dj, double li, double lj, 
			double eps0, double epsE, double epsS, double dw,
			const CutoffList& rc, double mu = 2.0, double nu = 1.0) : 
		_sig0(0), _X(0), _Xasq(0), _Xa_sq(0), _Xsq(0), _Xpsq(0), _Xpapsq(0), _Xpap_sq(0),
		_di(di), _dj(dj), _li(li), _lj(lj), _mu(mu), _nu(nu), _eps0(eps0), 
		_epsE(epsE), _epsS(epsS), _dw(dw), _rc(rc)
		{
			using std::pow;

			// Compute commonly used parameters.
			_sig0 = 0.5*(di + dj);
			_X = sqrt(((li*li-di*di)*(lj*lj-dj*dj))/((lj*lj+di*di)*(li*li+dj*dj)));
			_asq = sqrt(((li*li-di*di)*(lj*lj+di*di))/((lj*lj-dj*dj)*(li*li+dj*dj)));
			_Xasq = (li*li-di*di)/(li*li+dj*dj);
			_Xa_sq = (lj*lj-dj*dj)/(lj*lj+di*di);
			_Xsq = _X*_X;
			_Xpsq = pow((1-pow(epsE/epsS, 1./mu))/(1+pow(epsE/epsS, 1./mu)),2.);
			_Xpapsq = (pow(epsS, 1./mu)-pow(epsE, 1./mu))/(pow(epsS, 1./mu)+pow(epsE, 1./mu));
			_Xpap_sq = (pow(epsS, 1./mu)-pow(epsE, 1./mu))/(pow(epsS, 1./mu)+pow(epsE, 1./mu));
		}

		virtual Interaction Evaluate(
			const Particle& p1, 
			const Particle& p2, 
			const Position& rij, 
			unsigned int wid) const override
		{
			Interaction ep;

			auto r = fnorm(rij);
			if(r > _rc[wid])
				return ep;
			
			auto& ui = p1.GetDirector();
			auto& uj = p2.GetDirector();

			auto uirij = fdot(ui, rij)/r;
			auto ujrij = fdot(uj, rij)/r;
			auto uiuj = fdot(ui, uj);

			auto H =  (_Xasq*uirij*uirij   + _Xa_sq*ujrij*ujrij   - 2.*_Xsq*uirij*ujrij*uiuj)/(1.  - _Xsq*uiuj*uiuj);
			auto Hp = (_Xpapsq*uirij*uirij + _Xpap_sq*ujrij*ujrij - 2.*_Xpsq*uirij*ujrij*uiuj)/(1. - _Xpsq*uiuj*uiuj);
			auto sig = _sig0/sqrt(1.0-H);
			auto eps1 = 1.0/sqrt(1.0-_Xsq*uiuj*uiuj);
			auto eps2 = 1.0 - Hp;
			auto eps = _eps0*pow(eps1, _nu)*pow(eps2, _mu);
			auto R = _dw*_sig0/(r - sig + _dw*_sig0);
			ep.energy = 4.0*eps*(pow(R, 12.) - pow(R, 6.));

			// Another check for unphysicalness. R is an approximation 
			// of the surface to surface distance. It should never be 
			// negative.
			if(R < 0)
			{
				printf("inside GayBerne->Evaluate: unphysical value R=%f, r=%f\n", R, r);
				// return {1.0e10/std::abs(r), 0};
				return {1.0e6/std::abs(r), 0};
			}
			return ep;
		}

		virtual double EvaluateTorque(
			const Particle& p1, 
			const Particle& p2, 
			const Position& rij, 
			unsigned int wid) const override
		{
			double torque = 0.0;

			auto r = fnorm(rij);
			if(r > _rc[wid])
				return torque;
			
			auto& ui = p1.GetDirector();
			auto& uj = p2.GetDirector();

			auto uirij = fdot(ui, rij)/r;
			auto ujrij = fdot(uj, rij)/r;
			auto uiuj = fdot(ui, uj);

			auto H =  (_Xasq*uirij*uirij   + _Xa_sq*ujrij*ujrij   - 2.*_Xsq*uirij*ujrij*uiuj)/(1.  - _Xsq*uiuj*uiuj);
			auto Hp = (_Xpapsq*uirij*uirij + _Xpap_sq*ujrij*ujrij - 2.*_Xpsq*uirij*ujrij*uiuj)/(1. - _Xpsq*uiuj*uiuj);
			auto sig = _sig0/sqrt(1.0-H);
			auto eps1 = 1.0/sqrt(1.0-_Xsq*uiuj*uiuj);
			auto eps2 = 1.0 - Hp;
			auto eps = _eps0*pow(eps1, _nu)*pow(eps2, _mu);
			auto R = _dw*_sig0/(r - sig + _dw*_sig0);
			auto R3 = R*R*R;
			auto R6 = R3*R3;
			auto R7 = R6*R;
			auto R12 = R6*R6;
			auto R13 = R6*R7;

			auto sig3 = sig*sig*sig;
			auto sig03 = _sig0*_sig0*_sig0;
			auto pref1 = - 8.0 * eps * _mu * (R12 - R6) / (eps2 * r);
			auto pref2 = 8.0 * eps * sig3 * (6.0*R13 - 3.0*R7) / (_dw * r * sig03);
	
			auto dUda = pref1 * (_Xpapsq*uirij - _Xpsq*ujrij*uiuj) / (1.0 - _Xpsq*uiuj*uiuj) +
      					pref2 * (_Xasq * uirij - _Xasq *ujrij*uiuj) / (1.0 - _Xsq * uiuj*uiuj);

			auto dUdg = 4.0 * eps * _nu * (R12 - R6) * _Xsq * uiuj / (1.0 - _Xsq*uiuj*uiuj) +
      					8.0 * eps * _mu * (R12 - R6) * (_Xpsq*uirij*ujrij - Hp*_Xpsq*uiuj) / 
      					(1.0 - _Xpsq * uiuj*uiuj) / eps2 + 8.0 * eps * sig3 * (3.0 * R7 - 6.0 * R13) * 
      					(_Xsq * uirij * ujrij - H * _Xsq * uiuj) / (1.0 - _Xsq * uiuj*uiuj) / (_dw * sig03);

			auto rxu = rij[0]*ui[1] - rij[1]*ui[0];
			auto uxu = ui[0]*uj[1] - ui[1]*uj[0];

			//ep.energy = 4.0*eps*(pow(R, 12.) - pow(R, 6.));
			torque = ( dUda * rxu - dUdg * uxu );

			// Another check for unphysicalness. R is an approximation 
			// of the surface to surface distance. It should never be 
			// negative.
			if(R < 0)
			{
				printf("inside GayBerne->Evaluate: unphysical value R=%f, r=%f\n", R, r);
				// return {1.0e10/std::abs(r), 0};
				return 0.0;
			}
			//printf("inside GayBerne->Evaluate: u1 = (%f, %f), u2 = (%f, %f), r = (%f, %f), R=%f, torque=%f\n", 
			//	ui[0], ui[1], uj[0], uj[1], rij[0], rij[1], R, torque);
			//printf("%f, %f, %f, %f, %f, %f, %f, %f\n", 
			//	ui[0], ui[1], uj[0], uj[1], rij[0], rij[1], R, torque);
			return torque;
		}

		virtual void Serialize(Json::Value& json) const override
		{
			json["type"] = "GayBerne"; 
			json["diameters"][0] = _di;
			json["diameters"][1] = _dj;
			json["lengths"][0] = _li;
			json["lengths"][1] = _lj;
			json["eps0"] = _eps0;
			json["epsE"] = _epsE;
			json["epsS"] = _epsS;
			json["mu"] = _mu;
			json["nu"] = _nu;
			json["dw"] = _dw;
			for(auto& rc : _rc)
				json["rcut"].append(rc);
		}

		~GayBerneFF(){}
	};
}
