#pragma once 
#include <cstdio>
#include <cerrno>
#include <math.h>
#include "../Properties/Vector2D.h"

// This a collection of random helper functions.
namespace SAPHRON
{
	template <typename T> int sgn(T val) 
	{
	    return (T(0) < val) - (val < T(0));
	}

	// Create a rotation matrix which will rotate a vector 
	// abount an axis (x = 1, y = 2, z = 3) "deg" degrees.
	inline Matrix2D GenRotationMatrix(double deg)
	{
		double psi = deg;

		// Compote trig functions once.
		auto cpsi = cos(psi);
		auto spsi = sin(psi);

		// Build rotation matrix. 
		return {{ cpsi, spsi},
				{-spsi, cpsi}};
	}

	// Retrieves the contents of a file and returns them
	// in a string. Throws exception on failure.
	inline std::string GetFileContents(const char *filename)
	{
		std::FILE *fp = std::fopen(filename, "rb");
		if (fp)
		{
			std::string contents;
			std::fseek(fp, 0, SEEK_END);
			contents.resize(std::ftell(fp));
			std::rewind(fp);

			// Stupid GCC bug. We do this to hide warnings.
			if(!std::fread(&contents[0], 1, contents.size(), fp))
				std::fclose(fp);
			else
				std::fclose(fp);

			return(contents);
		}
		throw(errno);
	}

	// Gets file path from filename.
	inline std::string GetFilePath(const std::string& str)
	{
		size_t found;
		found = str.find_last_of("/\\");
		if(found == str.npos)
			return "./";
		return str.substr(0, found);
	}
}