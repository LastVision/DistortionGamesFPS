#include "stdafx.h"
#include <ActivateSpawnpointMessage.h>
#include "Mission.h"
#include <NetMessageSetActive.h>
#include <NetMessageDisplayMarker.h>
#include <NetMessageText.h>
#include <PostMaster.h>
#include "ServerNetworkManager.h"
#include <SetActiveMessage.h>
#include <SendTextToClientsMessage.h>

Mission::Mission(const std::string& aMissionType, int aMissionID, bool aShouldLoopMissionEvents)
	: myShouldLoopMissionEvents(aShouldLoopMissionEvents)
	, myStartEvents(8)
	, myMissionEvents(8)
	, myEndEvents(8)
	, myCurrentMissionEvent(0)
	, myMissionID(aMissionID)
{
	if (aMissionType == "killx")
	{
		myMissionType = eMissionType::KILL_X;
	}
	else if (aMissionType == "defend")
	{
		myMissionType = eMissionType::DEFEND;
	}
	else if (aMissionType == "event")
	{
		myMissionType = eMissionType::EVENT;
	}
	else
	{
		DL_ASSERT("UNKNOWN MISSIONTYPE");
	}
}

Mission::~Mission()
{
}

void Mission::SendMissionMessage(ActionEvent anEvent)
{
	switch (anEvent.myType)
	{
	case eActionEventType::LOCK:
		ServerNetworkManager::GetInstance()->AddMessage(NetMessageSetActive(true, true, anEvent.myGID));
		PostMaster::GetInstance()->SendMessage(SetActiveMessage(anEvent.myGID, true));
		break;
	case eActionEventType::UNLOCK:
		ServerNetworkManager::GetInstance()->AddMessage(NetMessageSetActive(false, true, anEvent.myGID));
		PostMaster::GetInstance()->SendMessage(SetActiveMessage(anEvent.myGID, false));
		break;
	case eActionEventType::SPAWN:
		PostMaster::GetInstance()->SendMessage(ActivateSpawnpointMessage(anEvent.myGID));
		break;
	case eActionEventType::TEXT:
		ServerNetworkManager::GetInstance()->AddMessage(NetMessageText(anEvent.myText, anEvent.myShowTextTime
			, { 1.f, 1.f, 1.f, 1.f }, anEvent.myTextRows, false, true, anEvent.mySoundEvent));
		//PostMaster::GetInstance()->SendMessage(SendTextToClientsMessage(anEvent.myText, anEvent.myShowTextTime));
		break;
	case eActionEventType::MARKER:
		ServerNetworkManager::GetInstance()->AddMessage(NetMessageDisplayMarker(anEvent.myPosition, anEvent.myShow));
		break;
	default:
		break;
	}
}

void Mission::AddStartEvent(ActionEvent aEvent)
{
	myStartEvents.Add(aEvent);
}

void Mission::AddMissionEvent(ActionEvent aEvent)
{
	myMissionEvents.Add(aEvent);
}

void Mission::AddEndEvent(ActionEvent aEvent)
{
	myEndEvents.Add(aEvent);
}