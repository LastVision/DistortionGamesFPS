#pragma once
#include <GrowingArray.h>

class ServerState;
class ServerStateStackProxy;

class ServerStateStack
{
public:
	ServerStateStack();
	~ServerStateStack();

	void PushSubState(ServerState* aSubState);
	void PushMainState(ServerState* aMainState);
	bool UpdateCurrentState(const float aDeltaTime);
	void ResumeState();
	void Clear();
private:
	void PopSubState();
	void PopMainState();

	int myMainIndex;
	int mySubIndex;

	ServerStateStackProxy* myStateStackProxy;
	CU::GrowingArray<CU::GrowingArray<ServerState*>> myStates;
};

