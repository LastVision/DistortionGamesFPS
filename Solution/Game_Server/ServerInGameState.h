#pragma once
#include "ServerState.h"

class ServerLevel;
class ServerLevelFactory;

class ServerInGameState : public ServerState
{
public:
	ServerInGameState(int aLevelID);
	~ServerInGameState();

	void InitState(ServerStateStackProxy* aStateStackProxy) override;
	void EndState() override;

	const eStateStatus Update(const float aDeltaTime) override;
	void ResumeState() override;
private:
	ServerLevel* myLevel;
	ServerLevelFactory* myLevelFactory;
	int myLevelID;
};

