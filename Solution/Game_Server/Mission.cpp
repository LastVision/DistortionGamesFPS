#include "stdafx.h"
#include "Mission.h"
#include <NetMessageSetActive.h>
#include <PostMaster.h>
#include "ServerNetworkManager.h"
#include <SetActiveMessage.h>

Mission::Mission(const std::string& aMissionType, bool aShouldLoopMissionEvents)
	: myMissionType(aMissionType)
	, myShouldLoopMissionEvents(aShouldLoopMissionEvents)
	, myStartEvents(8)
	, myMissionEvents(8)
	, myEndEvents(8)
	, myCurrentMissionEvent(0)
{
}


Mission::~Mission()
{
}

void Mission::SendMissionMessage(eActionEventType aType, int aGID)
{
	switch (aType)
	{
	case eActionEventType::LOCK:
		ServerNetworkManager::GetInstance()->AddMessage(NetMessageSetActive(true, aGID));
		//myActiveEntitiesMap[aGid]->GetComponent<PhysicsComponent>()->AddToScene();
		PostMaster::GetInstance()->SendMessage(SetActiveMessage(aGID, true));
		//Postmastermeddelande till level om att lägga till i scen
		break;
	case eActionEventType::UNLOCK:
		ServerNetworkManager::GetInstance()->AddMessage(NetMessageSetActive(false, aGID));
		//myActiveEntitiesMap[aGid]->GetComponent<PhysicsComponent>()->RemoveFromScene();
		PostMaster::GetInstance()->SendMessage(SetActiveMessage(aGID, false));
		//Postmastermeddelande till level om att ta bort ur scene
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