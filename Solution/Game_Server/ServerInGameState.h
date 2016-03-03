#pragma once
#include "ServerState.h"
class ServerInGameState : public ServerState
{
public:
	ServerInGameState();
	~ServerInGameState();

	void InitState(ServerStateStackProxy* aStateStackProxy) override;
	void EndState() override;

	const eStateStatus Update(const float aDeltaTime) override;
	void ResumeState() override;
};

