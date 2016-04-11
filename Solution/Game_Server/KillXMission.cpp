#include "stdafx.h"
#include "KillXMission.h"
#include <SendTextToClientsMessage.h>
#include <PostMaster.h>

KillXMission::KillXMission(const std::string& aMissionType, int aMissionID, int aEnemiesToKill, bool aShouldLoopMissionEvents)
	: Mission(aMissionType, aMissionID, aShouldLoopMissionEvents)
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
			SendMissionMessage(myStartEvents[0]);
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
				SendMissionMessage(myMissionEvents[myCurrentMissionEvent]);
				++myCurrentMissionEvent;
				if (myCurrentMissionEvent == myMissionEvents.Size())
				{
					if (myShouldLoopMissionEvents == true)
					{
						myCurrentMissionEvent = 0;
						for (int i = 0; i < myMissionEvents.Size(); ++i)
						{
							myMissionEvents[i].myTimeBeforeStarting = myMissionEvents[i].myResetTime;
						}
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
			SendMissionMessage(myEndEvents[0]);
			myEndEvents.RemoveNonCyclicAtIndex(0);
		}
		return false;
	}
	//PostMaster::GetInstance()->SendMessage<SendTextToClientsMessage>(SendTextToClientsMessage("Mission complete"));

	return true;
}

void KillXMission::AddValue(int aValue)
{
	myEnemiesKilled += aValue;
	if (myEnemiesKilled <= myAmountToKill)
	{
		PostMaster::GetInstance()->SendMessage<SendTextToClientsMessage>(SendTextToClientsMessage("Enemies left: " + std::to_string(myAmountToKill - myEnemiesKilled)));
	}
	printf("Enemy killed! %i / %i \n", myEnemiesKilled, myAmountToKill);
}