#pragma once
#include <GameEnum.h>
#include <NetworkSubscriber.h>
#include "ServerState.h"

class ServerLevel;
class ServerLevelFactory;

class ServerInGameState : public ServerState, public NetworkSubscriber
{
public:
	ServerInGameState(int aLevelID);
	~ServerInGameState();

	void InitState(ServerStateStackProxy* aStateStackProxy) override;
	void EndState() override;

	const eStateStatus Update(const float aDeltaTime) override;
	void ResumeState() override;

	void ReceiveNetworkMessage(const NetMessageLevelComplete& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageRequestStartLevel& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageLevelLoaded& aMessage, const sockaddr_in& aSenderAddress) override;


private:
	void LevelUpdate(float aDeltaTime);
	ServerLevel* myLevel;
	ServerLevelFactory* myLevelFactory;
	int myLevelID;

	eInGameStates myState;

	CU::GrowingArray<unsigned int> myRespondedClients;
};

