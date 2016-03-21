#include "stdafx.h"
#include "DefendMission.h"


DefendMission::DefendMission(const std::string& aMissionType, float aSecondsToDefend, bool aShouldLoopMissionEvents)
	: Mission(aMissionType, aShouldLoopMissionEvents)
	, myDefendTime(aSecondsToDefend)
	, myShouldCountDown(true)
	, myEnemiesInside(0)
{
}

DefendMission::~DefendMission()
{
}

bool DefendMission::Update(float aDeltaTime)
{
	if (myStartEvents.Size() > 0)
	{
		myStartEvents[0].myTimeBeforeStarting -= aDeltaTime;
		if (myStartEvents[0].myTimeBeforeStarting <= 0.f)
		{
			SendMissionMessage(myStartEvents[0].myType, myStartEvents[0].myGID);
			myStartEvents.RemoveNonCyclicAtIndex(0);
		}
		return false;
	}
	else if (myDefendTime >= 0.f)
	{
		if (myEnemiesInside <= 0)
		{
			myDefendTime -= aDeltaTime;
		}
		return false;
	}
	else if (myEndEvents.Size() > 0)
	{
		myEndEvents[0].myTimeBeforeStarting -= aDeltaTime;
		if (myEndEvents[0].myTimeBeforeStarting <= 0.f)
		{
			SendMissionMessage(myEndEvents[0].myType, myEndEvents[0].myGID);
			myEndEvents.RemoveNonCyclicAtIndex(0);
		}
		return false;
	}

	return true;
}

void DefendMission::AddValue(int aValue)
{
	myEnemiesInside += aValue;
}
