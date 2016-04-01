#pragma once
#include "Mission.h"

class KillXMission : public Mission
{
public:
	KillXMission(const std::string& aMissionType, int aEnemiesToKill, bool aShouldLoopMissionEvents);
	~KillXMission();

	bool Update(float aDeltaTime) override;
	void AddValue(int aValue) override;

private:
	int myEnemiesKilled;
	int myAmountToKill;

};

