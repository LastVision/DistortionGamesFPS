#include "stdafx.h"
#include "BaseNetwork.h"
#include <DL_Assert.h>

BaseNetwork::BaseNetwork()
{
}


BaseNetwork::~BaseNetwork()
{
}

void BaseNetwork::ConnectToServer(const char*)
{
	DL_ASSERT("Tried to call \"Connect to Server\" outside Client!");
}
