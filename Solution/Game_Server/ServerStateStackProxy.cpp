#include "stdafx.h"
#include "ServerStateStack.h"
#include "ServerStateStackProxy.h"

ServerStateStackProxy::ServerStateStackProxy(ServerStateStack& aStateStack) : myStateStack(aStateStack)
{
}

void ServerStateStackProxy::PushSubState(ServerState* aState)
{
	myStateStack.PushSubState(aState);
}

void ServerStateStackProxy::PushMainState(ServerState* aState)
{
	myStateStack.PushMainState(aState);
}