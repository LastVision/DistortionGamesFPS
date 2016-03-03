#pragma once
#include <string>
namespace Prism
{
	struct AnimationData
	{
		AnimationData()
			: myFile("Not loaded")
			, myShouldLoop(false)
			, myResetTimeOnRestart(false)
			, myElapsedTime(0)
		{}
		std::string myFile;
		bool myShouldLoop;
		bool myResetTimeOnRestart;
		float myElapsedTime;
	};
}