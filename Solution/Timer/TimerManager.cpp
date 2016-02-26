#include "stdafx.h"

#include <assert.h>
#include "Time.h"
#include "TimerManager.h"
#include <Windows.h>

CU::TimerManager* CU::TimerManager::myInstance = nullptr;

void CU::TimerManager::Create()
{
	myInstance = new TimerManager();
}

void CU::TimerManager::Destroy()
{
	delete myInstance;
}

CU::TimerManager* CU::TimerManager::GetInstance()
{
	return myInstance;
}

CU::TimerManager::TimerManager()
{
	LARGE_INTEGER largeInteger;
	QueryPerformanceFrequency(&largeInteger);
	myFrequency = largeInteger.QuadPart;

	QueryPerformanceCounter(&largeInteger);
	myLastTime = largeInteger.QuadPart * 1000000 / myFrequency;
}


CU::TimerManager::~TimerManager()
{
}

void CU::TimerManager::Update()
{

	TimeUnit time = GetTime();

	myMasterTimer.AddTime(time);

	for (unsigned int i = 0; i < myTimerList.size(); ++i)
	{
		myTimerList[i].AddTime(time);
	}


	myLastTime += time;
}

void CU::TimerManager::StartTimer(const std::string& aName)
{
	LARGE_INTEGER current;
	QueryPerformanceCounter(&current);

	myTimers[aName] = current.QuadPart * 1000000 / myFrequency;
}

CU::Time CU::TimerManager::StopTimer(const std::string& aName)
{
	if (myTimers.find(aName) == myTimers.end())
		return Time(0, 0);

	LARGE_INTEGER current;
	QueryPerformanceCounter(&current);

	TimeUnit resultTime = (current.QuadPart * 1000000 / myFrequency) - myTimers[aName];

	return Time(resultTime, 0);
}

const CU::Timer& CU::TimerManager::GetMasterTimer() const
{
	return myMasterTimer;
}

CU::TimeUnit CU::TimerManager::GetTime()
{
	LARGE_INTEGER current;
	QueryPerformanceCounter(&current);

	return (current.QuadPart * 1000000 / myFrequency) - myLastTime;
}

CU::TimerHandle CU::TimerManager::CreateTimer()
{
	myTimerList.push_back(Timer());
	return myTimerList.size() - 1;
}


const CU::Timer& CU::TimerManager::GetTimer(CU::TimerHandle aId) const
{
	assert(aId < myTimerList.size() && "GetTimer handle out of bounds.");
	return myTimerList[aId];
}


void CU::TimerManager::PauseAll()
{
	for (unsigned int i = 0; i < myTimerList.size(); ++i)
	{
		myTimerList[i].Pause();
	}
}

void CU::TimerManager::StartAll()
{
	for (unsigned int i = 0; i < myTimerList.size(); ++i)
	{
		myTimerList[i].Start();
	}
}

void CU::TimerManager::CapFrameRate(float aFrameRate)
{
	float capTime = 1.f / aFrameRate;

	float waitTime = 0;

	while (waitTime < capTime)
	{
		float frameTime = float(GetTime());
		frameTime /= 1000000.f;
		waitTime = frameTime;
	}
}