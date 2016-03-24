#include "stdafx.h"
#include "KillXMission.h"

KillXMission::KillXMission(const std::string& aMissionType, int aEnemiesToKill, bool aShouldLoopMissionEvents)
	: Mission(aMissionType, aShouldLoopMissionEvents)
	, myAmountToKill(aEnemiesToKill)
{
}

KillXMission::~KillXMission()
{
}

bool KillXMission::Update(float aDeltaTime)
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
	else if (myEnemiesKilled < myAmountToKill)
	{
		if (myMissionEvents.Size() > 0)
		{
			myMissionEvents[myCurrentMissionEvent].myTimeBeforeStarting -= aDeltaTime;
			if (myMissionEvents[myCurrentMissionEvent].myTimeBeforeStarting <= 0.f)
			{
				SendMissionMessage(myMissionEvents[myCurrentMissionEvent].myType, myMissionEvents[myCurrentMissionEvent].myGID);
				++myCurrentMissionEvent;
				if (myCurrentMissionEvent == myMissionEvents.Size())
				{
					if (myShouldLoopMissionEvents == true)
					{
						myCurrentMissionEvent = 0;
					}
					else
					{
						myMissionEvents.RemoveAll();
					}
				}
			}
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

void KillXMission::AddValue(int aValue)
{
	myEnemiesKilled += aValue;
}