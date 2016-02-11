#pragma once
#include "Vector.h"

namespace CU
{
	namespace Intersection
	{
		struct AABB
		{
			CU::Vector3<float> myCenterPos;
			CU::Vector3<float> myExtents;
			CU::Vector3<float> myMinPos;
			CU::Vector3<float> myMaxPos;
		};
	}
}