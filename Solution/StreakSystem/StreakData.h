#pragma once
#include <Vector.h>

namespace Prism
{
	struct StreakData
	{
		bool myStreaksFollowEmitter;
		bool myAffectedByGravity;
		float myAlphaDelta;
		float myLifeTime;
		float myMaxStartSize;
		float myMinStartSize;
		CU::Vector3f myMaxVelocity;
		CU::Vector3f myMinVelocity;
		float mySizeDelta;
		float myStartAlpha;
	};
}