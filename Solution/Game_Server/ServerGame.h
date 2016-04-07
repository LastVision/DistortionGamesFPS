#pragma once
#include <NetworkSubscriber.h>
#include "ServerStateStack.h"
#include <SharedGame.h>

class ServerGame : public SharedGame, public NetworkSubscriber
{
public:
	ServerGame();
	~ServerGame();
	bool Init();
	bool Update() override;

	void ReceiveNetworkMessage(const NetMessageKillServer& aMessage, const sockaddr_in& aSenderAddress) override;
private:
	ServerStateStack myStateStack;
	float myDeltaTime;
	float myTextUpdate;
	bool myIsQuiting;
};

