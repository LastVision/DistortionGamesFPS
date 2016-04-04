#pragma once
#include <NetworkSubscriber.h>
#include "ServerState.h"
class ServerStartState : public ServerState, public NetworkSubscriber
{
public:
	ServerStartState();
	~ServerStartState();

	void InitState(ServerStateStackProxy* aStateStackProxy) override;
	void EndState() override;

	const eStateStatus Update(const float aDeltaTime) override;
	void ResumeState() override;

	void ReceiveNetworkMessage(const NetMessageRequestConnect& aMessage, const sockaddr_in& aSenderAddress) override;


private:

};

