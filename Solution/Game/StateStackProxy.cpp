#include "stdafx.h"
#include "StateStackProxy.h"
#include "StateStack.h"
#include "GameState.h"

StateStackProxy::StateStackProxy(StateStack& aStateStack) : myStateStack(aStateStack)
{
}

void StateStackProxy::PushSubGameState(GameState* aGameState)
{
	myStateStack.PushSubGameState(aGameState);
}

void StateStackProxy::PushMainGameState(GameState* aGameState)
{
	myStateStack.PushMainGameState(aGameState);
}
