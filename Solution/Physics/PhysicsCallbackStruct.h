#pragma once
#include "PhysicsComponentData.h"
#include <functional>

namespace Prism
{
	struct PhysicsCallbackStruct
	{
		PhysicsCallbackStruct()
		: myData(nullptr)
		, mySwapOrientationCallback(nullptr)
		, myUpdateCallback(nullptr) 
		{}
		
		PhysicsCallbackStruct(const PhysicsComponentData& aData, std::function<void()> aSwap, std::function<void()> aUpdate)
			: myData(&aData)
			, mySwapOrientationCallback(aSwap)
			, myUpdateCallback(aUpdate)
		{}
		const PhysicsComponentData* myData;
		std::function<void()> mySwapOrientationCallback;
		std::function<void()> myUpdateCallback;
	};
}