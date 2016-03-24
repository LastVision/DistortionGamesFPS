#include "stdafx.h"
#include <CommonHelper.h>
#include "DefendMission.h"
#include <DefendTouchMessage.h>
#include <EnemyKilledMessage.h>
#include "KillXMission.h"
#include "Mission.h"
#include "MissionManager.h"
#include <PostMaster.h>
#include <XMLReader.h>

MissionManager::MissionManager(const std::string& aMissionXMLPath)
{
	LoadMissions(aMissionXMLPath);
	PostMaster::GetInstance()->Subscribe(eMessageType::ENEMY_KILLED, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::DEFEND_TOUCH, this);
}

MissionManager::~MissionManager()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ENEMY_KILLED, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::DEFEND_TOUCH, this);
	for (auto it = myMissions.begin(); it != myMissions.end(); ++it)
	{
		SAFE_DELETE(it->second);
	}
}

void MissionManager::Update(float aDeltaTime)
{
	if (myCurrentMission != nullptr)
	{
		if (myCurrentMission->Update(aDeltaTime) == true)
		{
			myCurrentMission = nullptr;
		}
	}
}

void MissionManager::SetMission(int aId)
{
	myCurrentMission = myMissions[aId];
	if (myCurrentMission->GetMissionType() == eMissionType::DEFEND)
	{
		printf("Defend Mission started");
	}
	else if (myCurrentMission->GetMissionType() == eMissionType::KILL_X)
	{
		printf("KillX Mission started");
	}
}

eMissionType MissionManager::GetCurrentMissionType() const
{
	if (myCurrentMission != nullptr)
	{
		return myCurrentMission->GetMissionType();
	}
	return eMissionType::NONE;
}


void MissionManager::ReceiveMessage(const EnemyKilledMessage&)
{
	if (myCurrentMission != nullptr)
	{
		if (myCurrentMission->GetMissionType() == eMissionType::KILL_X)
		{
			myCurrentMission->AddValue(1);
		}
	}
}

void MissionManager::ReceiveMessage(const DefendTouchMessage& aMessage)
{
	if (myCurrentMission != nullptr)
	{
		if (myCurrentMission->GetMissionType() == eMissionType::DEFEND)
		{
			if (aMessage.myHasEntered == true)
			{
				myCurrentMission->AddValue(1);
			}
			else
			{
				myCurrentMission->AddValue(-1);
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
			KillXMission* mission = new KillXMission(missionType, enemiesToKill, shouldLoopMissionEvents);
			myMissions[missionId] = mission;
		}
		else if (missionType == "defend")
		{
			float secondsToDefend = 0;
			reader.ForceReadAttribute(rootElement, "value", secondsToDefend);
			DefendMission* mission = new DefendMission(missionType, secondsToDefend, shouldLoopMissionEvents);
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
			std::string eventType;
			reader.ForceReadAttribute(eventElement, "type", eventType);
			int gid = 0;
			reader.ForceReadAttribute(eventElement, "gid", gid);
			float timeBeforeStart = 0.f;
			reader.ForceReadAttribute(eventElement, "timeBeforeStarting", timeBeforeStart);
			myMissions[missionId]->AddStartEvent(ActionEvent(GetType(eventType), gid, timeBeforeStart));
		}

		eventElement = reader.FindFirstChild(rootElement, "MissionEvent");
		for (eventElement = reader.FindFirstChild(eventElement, "Event"); eventElement != nullptr;
			eventElement = reader.FindNextElement(eventElement, "Event"))
		{
			std::string eventType;
			reader.ForceReadAttribute(eventElement, "type", eventType);
			int gid = 0;
			reader.ForceReadAttribute(eventElement, "gid", gid);
			float timeBeforeStart = 0.f;
			reader.ForceReadAttribute(eventElement, "timeBeforeStarting", timeBeforeStart);
			myMissions[missionId]->AddMissionEvent(ActionEvent(GetType(eventType), gid, timeBeforeStart));
		}

		eventElement = reader.FindFirstChild(rootElement, "EndEvent");
		for (eventElement = reader.FindFirstChild(eventElement, "Event"); eventElement != nullptr;
			eventElement = reader.FindNextElement(eventElement, "Event"))
		{
			std::string eventType;
			reader.ForceReadAttribute(eventElement, "type", eventType);
			int gid = 0;
			reader.ForceReadAttribute(eventElement, "gid", gid);
			float timeBeforeStart = 0.f;
			reader.ForceReadAttribute(eventElement, "timeBeforeStarting", timeBeforeStart);
			myMissions[missionId]->AddEndEvent(ActionEvent(GetType(eventType), gid, timeBeforeStart));
		}
	}

	reader.CloseDocument();
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

	DL_ASSERT("UNKNOWN event type");
	return eActionEventType::LOCK;
}