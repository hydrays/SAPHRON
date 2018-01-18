#pragma once 

namespace SAPHRON
{
	struct Pressure
	{
		double ideal;
		double pxx;
		double pxy;
		double pyy;
		double ptail; 

		Pressure() : ideal(0), pxx(0), pxy(0), pyy(0), ptail(0){}
		Pressure(double idealp, double pxxp, double pxyp, 
				 double pyyp, double ptailp) : 
		ideal(idealp), pxx(pxxp), pxy(pxyp), pyy(pyyp), ptail(ptailp) {}

		// Calculate isotropic pressure.
		double isotropic()
		{
			return (pxx + pyy)/2.0 + ideal + ptail;
		}

		inline Pressure& operator+=(const Pressure& rhs)
		{
			pxx += rhs.pxx;
			pxy += rhs.pxy;
			pyy += rhs.pyy;
			ideal += rhs.ideal;
			ptail += rhs.ptail;
			return *this;
		}

		inline Pressure& operator-=(const Pressure& rhs)
		{
			pxx -= rhs.pxx;
			pxy -= rhs.pxy;
			pyy -= rhs.pyy;
			ideal -= rhs.ideal;
			ptail -= rhs.ptail;
			return *this;
		}

		inline Pressure& operator*=(const double& rhs)
		{
			pxx *= rhs;
			pxy *= rhs;
			pyy *= rhs;
			ideal *= rhs;
			ptail *= rhs;
			return *this;
		}

		inline Pressure& operator/=(const double& rhs)
		{
			pxx /= rhs;
			pxy /= rhs;
			pyy /= rhs;
			ideal /= rhs;
			ptail /= rhs;
			return *this;
		}

		inline Pressure operator*(const double& rhs) const
		{
			Pressure lhs(*this);
			lhs.pxx /= rhs;
			lhs.pxy /= rhs;
			lhs.pyy /= rhs;
			lhs.ideal /= rhs;
			lhs.ptail /= rhs;
			return lhs;
		}
	};

	inline Pressure operator+(Pressure lhs, const Pressure& rhs)
	{
		lhs.pxx += rhs.pxx;
		lhs.pxy += rhs.pxy;
		lhs.pyy += rhs.pyy;
		lhs.ideal += rhs.ideal;
		lhs.ptail += rhs.ptail;
		return lhs;
	}

	inline Pressure operator-(Pressure lhs, const Pressure& rhs)
	{
		lhs.pxx -= rhs.pxx;
		lhs.pxy -= rhs.pxy;
		lhs.pyy -= rhs.pyy;
		lhs.ideal -= rhs.ideal;
		lhs.ptail -= rhs.ptail;
		return lhs;
	}

	inline Pressure operator*(double lhs, const Pressure& rhs)
	{
		return rhs*lhs;
	}
}