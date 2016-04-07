#pragma once
#include "Mission.h"

class EventMission : public Mission
{
public:
	EventMission(const std::string& aMissionType, int aMissionID);
	~EventMission();

	bool Update(float aDeltaTime) override;
};

