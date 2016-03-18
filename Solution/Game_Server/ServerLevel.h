#pragma once
#include <SharedLevel.h>
class MissionManager;

class ServerLevel : public SharedLevel
{
public:
	ServerLevel();
	~ServerLevel();

	void Init() override;

	void Update(const float aDeltaTime) override;
	void CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond) override;

	void ReceiveNetworkMessage(const NetMessageRequestConnect& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageLevelLoaded& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveMessage(const SetActiveMessage& aMessage) override;

private:
	void HandleTrigger(Entity& aFirstEntity, Entity& aSecondEntity) override;
	unsigned int myEntityIDCount;
	CU::GrowingArray<unsigned int> myLoadedClients;
	bool myAllClientsLoaded;
	MissionManager* myMissionManager;
};

