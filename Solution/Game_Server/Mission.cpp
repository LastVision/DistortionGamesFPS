#include "stdafx.h"
#include "Mission.h"
#include <NetMessageSetActive.h>
#include <PostMaster.h>
#include "ServerNetworkManager.h"
#include <SetActiveMessage.h>

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

void Mission::SendMissionMessage(eActionEventType aType, int aGID)
{
	switch (aType)
	{
	case eActionEventType::LOCK:
		ServerNetworkManager::GetInstance()->AddMessage(NetMessageSetActive(true, true, aGID));
		PostMaster::GetInstance()->SendMessage(SetActiveMessage(aGID, true));
		break;
	case eActionEventType::UNLOCK:
		ServerNetworkManager::GetInstance()->AddMessage(NetMessageSetActive(false, true, aGID));
		PostMaster::GetInstance()->SendMessage(SetActiveMessage(aGID, false));
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