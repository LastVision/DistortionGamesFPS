#pragma once
#include "ServerState.h"
#include <NetworkSubscriber.h>

class ServerLobbyState : public ServerState, public NetworkSubscriber
{
public:
	ServerLobbyState();
	~ServerLobbyState();

	void InitState(ServerStateStackProxy* aStateStackProxy) override;
	void EndState() override;

	const eStateStatus Update(const float aDeltaTime) override;
	void ResumeState() override;

	void ReceiveNetworkMessage(const NetMessageSetLevel& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageRequestStartLevel& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageRequestConnect& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageRequestServer& aMessage, const sockaddr_in& aSenderAddress) override;

private:
	int myCurrentLevelID;
};

