#include "stdafx.h"

#include "Timer.h"

CU::Timer::Timer()
{
	myTotalTime = 0;
	myFrameTime = 0;
	myPaused = false;
}

CU::Timer::~Timer()
{
}

void CU::Timer::AddTime(TimeUnit aMicroSeconds)
{
	if (myPaused == false)
	{
		myTotalTime += aMicroSeconds;
		myFrameTime = aMicroSeconds;
	}
	else
	{
		myFrameTime = 0;
	}
}