#pragma once
#include "Mission.h"
class DefendMission : public Mission
{
public:
	DefendMission(const std::string& aMissionType, float aSecondsToDefend, bool aShouldLoopMissionEvents);
	~DefendMission();

	bool Update(float aDeltaTime) override;
	void AddValue(int aValue) override;

private:
	float myDefendTime;
	bool myShouldCountDown;
	int myEnemiesInside;
};

