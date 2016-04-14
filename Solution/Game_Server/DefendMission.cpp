#include "stdafx.h"
#include "DefendMission.h"
#include "NetMessageDisplayMarker.h"
#include <NetMessageText.h>
#include <SendTextToClientsMessage.h>
#include "ServerNetworkManager.h"
#include <PollingStation.h>
#include <PostMaster.h>

DefendMission::DefendMission(const std::string& aMissionType, int aMissionID, float aSecondsToDefend, bool aShouldLoopMissionEvents
	, const std::string& aProgressText, bool aProgressShouldCountDown)
	: Mission(aMissionType, aMissionID, aShouldLoopMissionEvents)
	, myDefendTime(aSecondsToDefend)
	, myMaxDefendTime(aSecondsToDefend)
	, myShouldCountDown(true)
	, myEnemiesInside(0)
	, myLastSecondToWarn(int(aSecondsToDefend))
	, mySendTime(2.f)
	, myProgressText(aProgressText)
	, myProgressShouldCountDown(aProgressShouldCountDown)
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
			int progress = (int((myDefendTime / myMaxDefendTime) * 100));
			if (myProgressShouldCountDown == false)
			{
				progress = 100 - progress;
			}
			ServerNetworkManager::GetInstance()->AddMessage(NetMessageText(myProgressText + std::to_string(progress) + " %", 1.f, { 1.f, 1.f, 1.f, 1.f }, 0, true, true));
			mySendTime = 2.f;
		}

		if (myDefendTime <= 0.f)
		{
			ServerNetworkManager::GetInstance()->AddMessage(NetMessageText("", 1.f, { 1.f, 1.f, 1.f, 1.f }, 0, true, false));
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
	PollingStation::GetInstance()->ResetEnemyTargetPosition();
	return true;
}

void DefendMission::AddValue(int aValue)
{
	std::string text = "Unit entered";
	CU::Vector4<float> color = { 0.7f, 0.2f, 0.2f, 1.f };

	if (aValue < 0)
	{
		text = "Unit left";
		color = { 0.2f, 0.7f, 0.2f, 1.f };
	}

	ServerNetworkManager::GetInstance()->AddMessage(NetMessageText(text, 4.f, color));

	myEnemiesInside += aValue;
	printf("%i inside defendtrigger \n", myEnemiesInside);
}
