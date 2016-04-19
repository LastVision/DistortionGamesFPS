#pragma once
#include "ServerState.h"
#include <NetworkSubscriber.h>

class ServerLevelFactory;

class ServerLobbyState : public ServerState, public NetworkSubscriber
{
public:
	ServerLobbyState(eGameType aGameType);
	~ServerLobbyState();

	void InitState(ServerStateStackProxy* aStateStackProxy) override;
	void EndState() override;

	const eStateStatus Update(const float aDeltaTime) override;
	void ResumeState() override;

	void ReceiveNetworkMessage(const NetMessageSetLevel& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageRequestLevel& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageRequestStartLevel& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageRequestConnect& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageRequestServer& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageDisconnect& aMessage, const sockaddr_in& aSenderAddress) override;

private:
	void operator=(const ServerLobbyState&) = delete;
	const eGameType myGameType;
	int myCurrentLevelID;
	ServerLevelFactory* myLevelFactory;
	bool myAboutToChangeState;
};

