#pragma once
#include <unordered_map>
class Mission;
class MissionManager
{
public:
	MissionManager();
	~MissionManager();

	void Update(float aDeltaTime);

	void SetMission(int aId);

private:
	Mission* myCurrentMission;
	std::unordered_map<int, Mission*> myMissions;
};

