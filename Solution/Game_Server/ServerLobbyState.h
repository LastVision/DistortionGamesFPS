#pragma once
#include "ServerState.h"
class ServerLobbyState : public ServerState
{
public:
	ServerLobbyState();
	~ServerLobbyState();

	void InitState(ServerStateStackProxy* aStateStackProxy) override;
	void EndState() override;

	const eStateStatus Update(const float aDeltaTime) override;
	void ResumeState() override;
private:
	
};

