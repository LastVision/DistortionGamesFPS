#include "stdafx.h"
#include <DL_Debug.h>
#include "ServerState.h"
#include "ServerStateStack.h"
#include "ServerStateStackProxy.h"

ServerStateStack::ServerStateStack()
{
	myMainIndex = -1;
	mySubIndex = -1;
	myStates.Init(4);

	myStateStackProxy = new ServerStateStackProxy(*this);
}

ServerStateStack::~ServerStateStack()
{
	delete myStateStackProxy;
	myStateStackProxy = nullptr;
}

void ServerStateStack::PushSubState(ServerState* aSubState)
{
	DL_ASSERT_EXP(myStates.Size() > 0, "Can't push sub game state, no main game state present.");
	DL_ASSERT_EXP(myMainIndex < 20 && mySubIndex < 20, "Can't add more than 20 states, it's unreasonable!");

	myStates[myMainIndex].Add(aSubState);
	aSubState->InitState(myStateStackProxy);

	SET_RUNTIME(true);

	mySubIndex = myStates[myMainIndex].Size() - 1;
}

void ServerStateStack::PushMainState(ServerState* aMainState)
{
	myStates.Add(CU::GrowingArray<ServerState*, int>(16));
	myMainIndex = myStates.Size() - 1;
	PushSubState(aMainState);
}

bool ServerStateStack::UpdateCurrentState(const float aDeltaTime)
{
	switch (myStates[myMainIndex][mySubIndex]->Update(aDeltaTime))
	{
	case eStateStatus::POP_SUB_STATE:
		PopSubState();
		break;
	case eStateStatus::POP_MAIN_STATE:
		PopMainState();
		break;
	case eStateStatus::KEEP_STATE:
		break;
	}

	return myStates.Size() > 0;
}

void ServerStateStack::ResumeState()
{
	myStates[myMainIndex][mySubIndex]->ResumeState();
}

void ServerStateStack::Clear()
{
	while (myStates.Size() > 0)
	{
		PopMainState();
	}
}

void ServerStateStack::PopSubState()
{
	DL_ASSERT_EXP(myStates.Size() > 0, "Can't pop an empty stack.");

	myStates[myMainIndex][mySubIndex]->EndState();

	myStates[myMainIndex].DeleteCyclicAtIndex(mySubIndex);
	--mySubIndex;

	if (mySubIndex >= 0)
	{
		myStates[myMainIndex][mySubIndex]->ResumeState();
	}
}

void ServerStateStack::PopMainState()
{
	while (myStates[myMainIndex].Size() > 0)
	{
		PopSubState();
	}

	myStates.RemoveCyclicAtIndex(myMainIndex);
	--myMainIndex;

	if (myMainIndex >= 0)
	{
		mySubIndex = myStates[myMainIndex].Size() - 1;
	}
	else
	{
		mySubIndex = -1;
	}

	if (myMainIndex >= 0 && mySubIndex >= 0)
	{
		myStates[myMainIndex][mySubIndex]->ResumeState();
	}
}