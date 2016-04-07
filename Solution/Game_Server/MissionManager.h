#pragma once
#include <GameEnum.h>
#include <unordered_map>
#include "ActionEvent.h"
#include <Subscriber.h>

class Mission;
class XMLReader;

namespace tinyxml2
{
	class XMLElement;
}

class MissionManager : public Subscriber
{
public:
	MissionManager(const std::string& aMissionXMLPath);
	~MissionManager();

	void Update(float aDeltaTime);

	void SetMission(int aId);
	eMissionType GetCurrentMissionType() const;

	void ReceiveMessage(const EnemyKilledMessage& aMessage) override;
	void ReceiveMessage(const DefendTouchMessage& aMessage) override;

private:
	eActionEventType GetType(const std::string& aType);
	void LoadMissions(const std::string& aMissionXMLPath);
	ActionEvent CreateActionEvent(tinyxml2::XMLElement* anEventElement, XMLReader* aReader);

	CU::GrowingArray<Mission*> myActiveMissionsQueue;

	std::unordered_map<int, Mission*> myMissions;
};