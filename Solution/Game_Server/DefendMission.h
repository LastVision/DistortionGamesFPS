#pragma once
#include "Mission.h"
class DefendMission : public Mission
{
public:
	DefendMission(const std::string& aMissionType, int aMissionID, float aSecondsToDefend, bool aShouldLoopMissionEvents
		, const std::string& aProgressText, bool aProgressShouldCountDown);
	~DefendMission();

	bool Update(float aDeltaTime) override;
	void AddValue(int aValue) override;

private:
	float myDefendTime;
	float myMaxDefendTime;
	float mySendTime;
	bool myShouldCountDown;
	int myEnemiesInside;
	int myLastSecondToWarn;

	std::string myProgressText;
	bool myProgressShouldCountDown;
};

