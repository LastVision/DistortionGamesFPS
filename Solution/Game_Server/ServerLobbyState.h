#pragma once
#include "ServerState.h"
#include <Subscriber.h>

class ServerLobbyState : public ServerState, public Subscriber
{
public:
	ServerLobbyState();
	~ServerLobbyState();

	void InitState(ServerStateStackProxy* aStateStackProxy) override;
	void EndState() override;

	const eStateStatus Update(const float aDeltaTime) override;
	void ResumeState() override;

	void ReceiveMessage(const PostMasterNetRequestStartGameMessage& aMessage) override;

private:
	int myCurrentLevelID;
};

