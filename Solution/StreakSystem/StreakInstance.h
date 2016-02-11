#pragma once

#include <Vector.h>

namespace Prism
{
	struct StreakInstance
	{
		CU::Vector3<float> myPosition;
		CU::Vector3<float> myVelocity;
		float mySize;
		float myAlpha;
		float myTime;
		float myRotation;
		float myRotationDelta;
	};
}