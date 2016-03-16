#pragma once
#include <SharedLevel.h>

class ServerLevel : public SharedLevel
{
public:
	ServerLevel();
	~ServerLevel();

	void Update(const float aDeltaTime) override;

	void ReceiveNetworkMessage(const NetMessageConnectMessage& aMessage, const sockaddr_in& aSenderAddress) override;
private:
	unsigned int myEntityIDCount;
	
};

