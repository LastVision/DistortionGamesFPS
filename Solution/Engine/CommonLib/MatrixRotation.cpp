#include "stdafx.h"
#include "MatrixRotation.h"

namespace CommonLib
{

	float ToRadians(float aDegree)
	{
		return aDegree*(PI/180.0f);
	};

	double ToRadiansD(double aDegree)
	{
		return aDegree*(dPI/180.0);
	};

	float ToDegrees(float aRadian)
	{
		return aRadian*(180.0f/PI);
	};

	double ToDegreesD(double aRadian)
	{
		return aRadian*(180.0/dPI);
	};
}
