#pragma once 

#include <armadillo>

namespace SAPHRON
{
	// Typedefs from arma.
	typedef arma::vec::fixed<2> Vector2D;
	typedef arma::mat22 Matrix2D;
	typedef Vector2D Director;
	typedef Vector2D Position;

	// Function to identify if two vecctors are close.
	inline bool is_close(const Vector2D& x, const Vector2D& y, double tol)
	{
		bool close(false);
		if(arma::norm(x-y, "inf") < tol)
			close  = true;

		return close;
	}

	// Fast norm.
	template<typename T>
	inline double fnorm(const T& v)
	{
		return sqrt(v[0]*v[0] + v[1]*v[1]);
	}

	// Fast dot.
	template<typename T1, typename T2>
	inline double fdot(const T1& v1, const T2& v2)
	{
		return v1[0]*v2[0] + v1[1]*v2[1];
	}
}