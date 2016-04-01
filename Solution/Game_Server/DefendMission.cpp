#include "stdafx.h"
#include "DefendMission.h"
#include "NetMessageDisplayMarker.h"
#include <NetMessageText.h>
#include <SendTextToClientsMessage.h>
#include "ServerNetworkManager.h"
#include <PollingStation.h>
#include <PostMaster.h>

DefendMission::DefendMission(const std::string& aMissionType, float aSecondsToDefend, bool aShouldLoopMissionEvents)
	: Mission(aMissionType, aShouldLoopMissionEvents)
	, myDefendTime(aSecondsToDefend)
	, myMaxDefendTime(aSecondsToDefend)
	, myShouldCountDown(true)
	, myEnemiesInside(0)
	, myLastSecondToWarn(int(aSecondsToDefend))
	, mySendTime(2.f)
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
			SendMissionMessage(myStartEvents[0]);
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

		mySendTime -= aDeltaTime;
		if (mySendTime <= 0.f)
		{
			int percentage = 100 - (int((myDefendTime / myMaxDefendTime) * 100));
			ServerNetworkManager::GetInstance()->AddMessage(NetMessageText("Virus upload: " + std::to_string(percentage) + " %", 1.f, true, true));
			mySendTime = 2.f;
		}

		if (myDefendTime <= 0.f)
		{
			ServerNetworkManager::GetInstance()->AddMessage(NetMessageText("", 1.f, true, false));
		}

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
		//test:
		//if (myLastSecondToWarn == int(myDefendTime))
		//{
		//	myLastSecondToWarn = int(myDefendTime);
		//	PostMaster::GetInstance()->SendMessage<SendTextToClientsMessage>(SendTextToClientsMessage("Hold out: " + std::to_string(myLastSecondToWarn), 1.f));
		//}
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
	//ServerNetworkManager::GetInstance()->AddMessage(NetMessageDisplayMarker({ 0.f, 0.f, 0.f }, false));
	PollingStation::GetInstance()->ResetEnemyTargetPosition();
	return true;
}

void DefendMission::AddValue(int aValue)
{
	std::string text = "A unit entered defend zone";

	if (aValue < 0)
	{
		text = "A unit left defend zone";
	}

	//PostMaster::GetInstance()->SendMessage<SendTextToClientsMessage>(SendTextToClientsMessage(text));
	ServerNetworkManager::GetInstance()->AddMessage(NetMessageText(text));

	myEnemiesInside += aValue;
	printf("%i inside defendtrigger \n", myEnemiesInside);
}
