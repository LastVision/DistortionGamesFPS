#pragma once
#include "StateEnums.h"
class ServerStateStackProxy;
class ServerState
{
public:
	virtual ~ServerState();

	virtual void InitState(ServerStateStackProxy* aStateStackProxy) = 0;
	virtual void EndState() = 0;

	virtual const eStateStatus Update(const float aDeltaTime) = 0;
	virtual void ResumeState() = 0;
protected:
	ServerStateStackProxy* myStateStack;
	eStateStatus myStateStatus;
	bool myIsActiveState;
};

inline ServerState::~ServerState()
{
}
