#pragma once

#include "Time.h"

namespace CU
{
	class Timer
	{
	public:
		Timer();
		~Timer();
		void AddTime(TimeUnit aMicroSeconds);

		Time GetTime() const;
		void Pause();
		void Start();
	private:
		bool myPaused;
		TimeUnit myTotalTime;
		TimeUnit myFrameTime;
	};
}

inline CU::Time CU::Timer::GetTime() const
{
	return CU::Time(myTotalTime, myFrameTime);
}

inline void CU::Timer::Pause()
{
	myPaused = true;
}

inline void CU::Timer::Start()
{
	myPaused = false;
}