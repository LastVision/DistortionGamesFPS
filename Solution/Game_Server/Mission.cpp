#include "stdafx.h"
#include "Mission.h"
#include <NetMessageSetActive.h>
#include <PostMaster.h>
#include "ServerNetworkManager.h"
#include <SetActiveMessage.h>
#include <SendTextToClientsMessage.h>

Mission::Mission(const std::string& aMissionType, bool aShouldLoopMissionEvents)
	: myShouldLoopMissionEvents(aShouldLoopMissionEvents)
	, myStartEvents(8)
	, myMissionEvents(8)
	, myEndEvents(8)
	, myCurrentMissionEvent(0)
{
	if (aMissionType == "killx")
	{
		myMissionType = eMissionType::KILL_X;
	}
	else if (aMissionType == "defend")
	{
		myMissionType = eMissionType::DEFEND;
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
	case eActionEventType::TEXT:
		PostMaster::GetInstance()->SendMessage(SendTextToClientsMessage(anEvent.myText, anEvent.myShowTextTime));
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