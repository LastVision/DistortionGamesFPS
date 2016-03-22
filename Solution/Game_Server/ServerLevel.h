#pragma once
#include <SharedLevel.h>
class MissionManager;

class ServerLevel : public SharedLevel
{
public:
	ServerLevel();
	~ServerLevel();

	void Init(const std::string& aMissionXMLPath) override;

	void Update(const float aDeltaTime) override;
	void CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond, bool aHasEntered) override;

	bool ChangeLevel(int& aNextLevel);

	void ReceiveNetworkMessage(const NetMessageLevelLoaded& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageEntityState& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageHealthPack& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveMessage(const SetActiveMessage& aMessage) override;

private:
	void HandleTrigger(Entity& aFirstEntity, Entity& aSecondEntity, bool aHasEntered) override;
	unsigned int myEntityIDCount;
	CU::GrowingArray<unsigned int> myLoadedClients;
	bool myAllClientsLoaded;
	MissionManager* myMissionManager;

	int myNextLevel;
};

