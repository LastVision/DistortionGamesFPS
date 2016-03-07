#include "stdafx.h"
#include "ConsoleBackspace.h"
#include <TimerManager.h>

ConsoleBackspace::ConsoleBackspace(std::string& aString)
	: myString(aString)
	, myFirstWait(0.5f)
	, myRepeatWait(0.15f)
	, myCurrentTime(0)
	, myHasReachedFirstWait(false)
{
}

void ConsoleBackspace::Update(bool aButtonIsPressed)
{
	if (aButtonIsPressed == false || myString.size() == 0)
	{
		myCurrentTime = 0;
		myHasReachedFirstWait = false;
		return;
	}
	float deltaTime = 1.f / 60.f; //CU::TimerManager::GetInstance()->GetMasterTimer().GetTime().GetFrameTime();
	
	myCurrentTime += deltaTime;
	if (aButtonIsPressed == true && myString.size() > 0 && myCurrentTime == 0 + deltaTime)
	{
		myString.erase(myString.end() - 1, myString.end());

		return;
	}

	if (myCurrentTime > myFirstWait)
	{
		myString.erase(myString.end() - 1, myString.end());
		myHasReachedFirstWait = true;
	}
	if (myHasReachedFirstWait == true && myCurrentTime > myRepeatWait + myFirstWait && myString.size() > 0)
	{
		myString.erase(myString.end() - 1, myString.end());
		myCurrentTime -= myRepeatWait;
	}
}