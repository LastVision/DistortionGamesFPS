#include "stdafx.h"
#include "MissionManager.h"

MissionManager::MissionManager()
{
	//Read missionxml
}

MissionManager::~MissionManager()
{
}

void MissionManager::Update(float aDeltaTime)
{
	aDeltaTime;
	//if (myCurrentMission != nullptr)
	//{
	//	myCurrentMission->Update(aDeltaTime);
	//	if (myCurrentMission->IsDone() == true)
	//	{
	//		myCurrentMission->EndMission();
	//		myCurrentMission->Reset();
	//		myCurrentMission = nullptr;
	//	}
	//}

}

void MissionManager::SetMission(int aId)
{
	myCurrentMission = myMissions[aId];
	//myCurrentMission->StartMission();
}