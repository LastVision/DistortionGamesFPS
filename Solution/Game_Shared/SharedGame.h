#pragma once

namespace CU
{
	class TimerManager;
}

class SharedGame
{
public:
	SharedGame();
	virtual ~SharedGame();
	
	virtual void ManualDestroy();

	virtual bool Update() = 0;

protected:
	CU::TimerManager* myTimerManager;
};

