#pragma once 

#include <armadillo>

namespace SAPHRON
{
	// Typedefs from arma.
	typedef arma::vec::fixed<3> Vector3D;
	typedef arma::mat33 Matrix3D;
	typedef Vector3D Director;
	typedef Vector3D Position;

	// Function to identify if two vecctors are close.
	inline bool is_close(const Vector3D& x, const Vector3D& y, double tol)
	{
		bool close(false);
		if(arma::norm(x-y, "inf") < tol)
			close  = true;

		return close;
	}
}