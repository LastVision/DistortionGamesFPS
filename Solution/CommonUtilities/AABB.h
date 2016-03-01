#pragma once
#include "Vector.h"

namespace CU
{
	namespace Intersection
	{
		struct AABB
		{
			AABB(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aScale)
				: myCenterPos(aPosition)
				, myExtents(aScale)
				, myMinPos(aPosition - aScale * 0.5f)
				, myMaxPos(aPosition + aScale * 0.5f)
			{}

			CU::Vector3<float> myCenterPos;
			CU::Vector3<float> myExtents;
			CU::Vector3<float> myMinPos;
			CU::Vector3<float> myMaxPos;
		};
	}
}