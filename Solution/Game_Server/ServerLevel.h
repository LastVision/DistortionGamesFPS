#pragma once
#include <SharedLevel.h>
class MissionManager;
class PhysicsComponent;

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
	void ReceiveNetworkMessage(const NetMessageShootGrenade& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessagePressE& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageRayCastRequest& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveMessage(const SendTextToClientsMessage& aMessage) override;
	void ReceiveMessage(const SetActiveMessage& aMessage) override;
	void ReceiveMessage(const RespawnMessage &aMessage) override;
	void ReceiveMessage(const RespawnTriggerMessage& aMessage) override;

	//void HandlePressedERaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal);

private:
	void HandleTrigger(Entity& aFirstEntity, Entity& aSecondEntity, bool aHasEntered) override;
	void Trigger(Entity& aFirstEntity, Entity& aSecondEntity, bool aHasEntered);
	void SendTextMessageToClients(const std::string& aText, float aTime = 5.f);
	//std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&, const CU::Vector3<float>&)> myPressedERayCastHandler;
	unsigned int myEntityIDCount;
	CU::GrowingArray<unsigned int> myLoadedClients;
	bool myAllClientsLoaded;
	MissionManager* myMissionManager;
	CU::GrowingArray<Entity*> myRespawnTriggers;

	std::unordered_map<int, CU::GrowingArray<Entity*>> myPlayersInPressableTriggers;

	int myNextLevel;
};

