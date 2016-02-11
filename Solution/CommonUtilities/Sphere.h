#pragma once
#include "Vector.h"
namespace CU
{
	namespace Intersection
	{
		class Sphere
		{
		public:
			CU::Vector3<float> myCenterPosition;
			float myRadius;
			float myRadiusSquared;
		};
	}
}