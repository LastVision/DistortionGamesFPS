#include "stdafx.h"
#include "EventMission.h"

EventMission::EventMission(const std::string& aMissionType, int aMissionID)
	: Mission(aMissionType, aMissionID, false)
{
}

EventMission::~EventMission()
{
}

bool EventMission::Update(float aDeltaTime)
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

	return true;
}