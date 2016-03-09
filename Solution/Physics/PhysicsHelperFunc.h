#pragma once

#include <foundation/PxVec3.h>
#include <foundation/PxTransform.h>
#include <Vector.h>

namespace Prism
{
	inline physx::PxVec3 ConvertVector(const CU::Vector3<float>& aVector)
	{
		return physx::PxVec3(aVector.x, aVector.y, aVector.z);
	}
}