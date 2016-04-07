#include "stdafx.h"
#include <CommonHelper.h>
#include "DefendMission.h"
#include <DefendTouchMessage.h>
#include <EnemyKilledMessage.h>
#include "EventMission.h"
#include "KillXMission.h"
#include "Mission.h"
#include "MissionManager.h"
#include <NetMessageDisplayMarker.h>
#include <PostMaster.h>
#include <SendTextToClientsMessage.h>
#include "ServerNetworkManager.h"
#include <XMLReader.h>

MissionManager::MissionManager(const std::string& aMissionXMLPath)
	: myActiveMissionsQueue(8)
{
	LoadMissions(aMissionXMLPath);
	PostMaster::GetInstance()->Subscribe(eMessageType::ENEMY_KILLED, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::DEFEND_TOUCH, this);
}

MissionManager::~MissionManager()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ENEMY_KILLED, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::DEFEND_TOUCH, this);
	myActiveMissionsQueue.RemoveAll();
	for (auto it = myMissions.begin(); it != myMissions.end(); ++it)
	{
		SAFE_DELETE(it->second);
	}
}

void MissionManager::Update(float aDeltaTime)
{
	if (myActiveMissionsQueue.Size() > 0)
	{
		if (myActiveMissionsQueue[0]->Update(aDeltaTime) == true)
		{
			myActiveMissionsQueue.RemoveNonCyclicAtIndex(0);
		}
	}
}

void MissionManager::SetMission(int aId)
{
	if (myActiveMissionsQueue.Size() == 0)
	{
		myActiveMissionsQueue.Add(myMissions[aId]);

#ifndef RELEASE_BUILD
		if (myMissions[aId]->GetMissionType() == eMissionType::DEFEND)
		{
			printf("Defend Mission started \n");
		}
		else if (myMissions[aId]->GetMissionType() == eMissionType::KILL_X)
		{
			printf("KillX Mission started \n");
		}
		else if (myMissions[aId]->GetMissionType() == eMissionType::EVENT)
		{
			printf("Event Mission started \n");
		}
#endif
	}
	else
	{
		for (int i = 0; i < myActiveMissionsQueue.Size(); i++)
		{
			if (myActiveMissionsQueue[i]->GetID() == aId)
			{
				return;
			}
		}

		myActiveMissionsQueue.Add(myMissions[aId]);

#ifndef RELEASE_BUILD
		if (myMissions[aId]->GetMissionType() == eMissionType::DEFEND)
		{
			printf("Defend Mission started \n");
		}
		else if (myMissions[aId]->GetMissionType() == eMissionType::KILL_X)
		{
			printf("KillX Mission started \n");
		}
		else if (myMissions[aId]->GetMissionType() == eMissionType::EVENT)
		{
			printf("Event Mission started \n");
		}
#endif
	}
}

eMissionType MissionManager::GetCurrentMissionType() const
{
	if (myActiveMissionsQueue.Size() > 0)
	{
		return myActiveMissionsQueue[0]->GetMissionType();
	}
	return eMissionType::NONE;
}

void MissionManager::ReceiveMessage(const EnemyKilledMessage&)
{
	if (myActiveMissionsQueue.Size() > 0)
	{
		if (myActiveMissionsQueue[0]->GetMissionType() == eMissionType::KILL_X)
		{
			myActiveMissionsQueue[0]->AddValue(1);
		}
	}
}

void MissionManager::ReceiveMessage(const DefendTouchMessage& aMessage)
{
	if (myActiveMissionsQueue.Size() > 0)
	{
		if (myActiveMissionsQueue[0]->GetMissionType() == eMissionType::DEFEND)
		{
			if (aMessage.myHasEntered == true)
			{
				myActiveMissionsQueue[0]->AddValue(1);
			}
			else
			{
				myActiveMissionsQueue[0]->AddValue(-1);
			}
		}
	}
}

void MissionManager::LoadMissions(const std::string& aMissionXMLPath)
{
	XMLReader reader;
	reader.OpenDocument(aMissionXMLPath);
	tinyxml2::XMLElement* rootElement = reader.ForceFindFirstChild("root");

	for (rootElement = reader.FindFirstChild(rootElement, "Mission"); rootElement != nullptr;
		rootElement = reader.FindNextElement(rootElement, "Mission"))
	{
		std::string missionType;
		reader.ForceReadAttribute(rootElement, "type", missionType);
		bool shouldLoopMissionEvents = false;
		reader.ForceReadAttribute(rootElement, "shouldLoopMissionEvents", shouldLoopMissionEvents);
		int missionId = -1;
		reader.ForceReadAttribute(rootElement, "id", missionId);
		missionType = CU::ToLower(missionType);
		if (missionType == "killx")
		{
			int enemiesToKill = 0;
			reader.ForceReadAttribute(rootElement, "value", enemiesToKill);
			KillXMission* mission = new KillXMission(missionType, missionId, enemiesToKill, shouldLoopMissionEvents);
			myMissions[missionId] = mission;
		}
		else if (missionType == "defend")
		{
			float secondsToDefend = 0;
			bool progressShouldCountDown = false;
			std::string progressText = "Progression: ";

			reader.ForceReadAttribute(rootElement, "value", secondsToDefend);
			reader.ReadAttribute(rootElement, "progressText", progressText);
			reader.ReadAttribute(rootElement, "progressShouldCountDown", progressShouldCountDown);

			DefendMission* mission = new DefendMission(missionType, missionId, secondsToDefend, shouldLoopMissionEvents, progressText, progressShouldCountDown);
			myMissions[missionId] = mission;
		}
		else if (missionType == "event")
		{
			EventMission* mission = new EventMission(missionType, missionId);
			myMissions[missionId] = mission;
		}
		else
		{
			DL_ASSERT("UNKNOWN MISSIONTYPE!");
		}

		tinyxml2::XMLElement* eventElement = reader.FindFirstChild(rootElement, "StartEvent");
		for (eventElement = reader.FindFirstChild(eventElement, "Event"); eventElement != nullptr;
			eventElement = reader.FindNextElement(eventElement, "Event"))
		{
			myMissions[missionId]->AddStartEvent(CreateActionEvent(eventElement, &reader));
		}

		eventElement = reader.FindFirstChild(rootElement, "MissionEvent");
		for (eventElement = reader.FindFirstChild(eventElement, "Event"); eventElement != nullptr;
			eventElement = reader.FindNextElement(eventElement, "Event"))
		{
			myMissions[missionId]->AddMissionEvent(CreateActionEvent(eventElement, &reader));
		}

		eventElement = reader.FindFirstChild(rootElement, "EndEvent");
		for (eventElement = reader.FindFirstChild(eventElement, "Event"); eventElement != nullptr;
			eventElement = reader.FindNextElement(eventElement, "Event"))
		{
			myMissions[missionId]->AddEndEvent(CreateActionEvent(eventElement, &reader));
		}
	}

	reader.CloseDocument();
}

ActionEvent MissionManager::CreateActionEvent(tinyxml2::XMLElement* anEventElement, XMLReader* aReader)
{
	ActionEvent actionEvent;
	std::string eventType;

	aReader->ForceReadAttribute(anEventElement, "type", eventType);
	aReader->ForceReadAttribute(anEventElement, "timeBeforeStarting", actionEvent.myTimeBeforeStarting);

	eActionEventType actionType = GetType(eventType);

	actionEvent.myType = actionType;

	switch (actionType)
	{
	case eActionEventType::LOCK:
	case eActionEventType::UNLOCK:
	case eActionEventType::SPAWN:
		aReader->ForceReadAttribute(anEventElement, "gid", actionEvent.myGID);
		break;
	case eActionEventType::TEXT:
		aReader->ForceReadAttribute(anEventElement, "timeForText", actionEvent.myShowTextTime);
		aReader->ForceReadAttribute(anEventElement, "text", actionEvent.myText);
		break;
	case eActionEventType::MARKER:
		aReader->ForceReadAttribute(anEventElement, "show", actionEvent.myShow);
		aReader->ReadAttribute(anEventElement, "positionx", actionEvent.myPosition.x);
		aReader->ReadAttribute(anEventElement, "positiony", actionEvent.myPosition.y);
		aReader->ReadAttribute(anEventElement, "positionz", actionEvent.myPosition.z);
		break;
	}
	actionEvent.myResetTime = actionEvent.myTimeBeforeStarting;

	return actionEvent;
}

eActionEventType MissionManager::GetType(const std::string& aType)
{
	if (aType == "remove")
	{
		return eActionEventType::UNLOCK;
	}
	else if (aType == "add")
	{
		return eActionEventType::LOCK;
	}
	else if (aType == "text")
	{
		return eActionEventType::TEXT;
	}
	else if (aType == "spawn")
	{
		return eActionEventType::SPAWN;
	}
	else if (aType == "marker")
	{
		return eActionEventType::MARKER;
	}

	DL_ASSERT("UNKNOWN event type");
	return eActionEventType::LOCK;
}