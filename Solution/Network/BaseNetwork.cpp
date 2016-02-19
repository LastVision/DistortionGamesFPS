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
	DL_ASSERT("Tried to call \"ConnectToServer\" outside Client!");
}

void BaseNetwork::CreateConnection(const std::string&)
{
	DL_ASSERT("Tried to call \"CreateConnection\" outside Server!");
}

void BaseNetwork::SetIsOnline(bool aIsOnline)
{
	myIsOnline = aIsOnline;
}

bool BaseNetwork::GetIsOnline() const
{
	return myIsOnline;
}
