#pragma once
#include "Mission.h"

class EventMission : public Mission
{
public:
	EventMission(const std::string& aMissionType);
	~EventMission();

	bool Update(float aDeltaTime) override;
};

