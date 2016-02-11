#pragma once
#include "Timer.h"
#include <vector>
#include <unordered_map>

namespace CU
{
	typedef unsigned int TimerHandle;

	class TimerManager
	{
	public:
		static void Create();
		static void Destroy();
		static TimerManager* GetInstance();

		void StartTimer(const std::string& aName);
		Time StopTimer(const std::string& aName);

		const Timer& GetTimer(TimerHandle aId) const;
		const Timer& GetMasterTimer() const;
		TimerHandle CreateTimer();
		void Update();
		void PauseAll();
		void StartAll();

		void CapFrameRate(float aFrameRate);
	private:
		TimerManager();
		~TimerManager();

		Timer myMasterTimer;
		TimeUnit GetTime();
		std::vector<Timer> myTimerList;
		TimeUnit myLastTime;
		unsigned long long myFrequency;

		std::unordered_map<std::string, TimeUnit> myTimers;

		static TimerManager* myInstance;
	};
}