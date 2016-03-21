#pragma once
#include <GrowingArray.h>
#include <GameEnum.h>

struct ActionEvent
{
	ActionEvent(){}
	ActionEvent(eActionEventType aType, int aGID, float aTimeBeforeStarting)
		: myType(aType)
		, myGID(aGID)
		, myTimeBeforeStarting(aTimeBeforeStarting)
	{}
	eActionEventType myType;
	int myGID;
	float myTimeBeforeStarting;
};

class Mission 
{
public:
	Mission(const std::string& aMissionType, bool aShouldLoopMissionEvents);
	virtual ~Mission();

	virtual bool Update(float aDeltaTime) = 0;

	void AddStartEvent(ActionEvent aEvent);
	void AddMissionEvent(ActionEvent aEvent);
	void AddEndEvent(ActionEvent aEvent);

	const eMissionType GetMissionType() const;
	virtual void AddValue(int aValue) = 0;

protected:
	void SendMissionMessage(eActionEventType aType, int aGID);
	CU::GrowingArray<ActionEvent> myStartEvents;
	CU::GrowingArray<ActionEvent> myMissionEvents;
	CU::GrowingArray<ActionEvent> myEndEvents;
	bool myShouldLoopMissionEvents;
	int myCurrentMissionEvent;
	eMissionType myMissionType;
};

inline const eMissionType Mission::GetMissionType() const
{
	return myMissionType;
}