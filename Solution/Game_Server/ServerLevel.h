#pragma once
#include <SharedLevel.h>

class ServerLevel : public SharedLevel
{
public:
	ServerLevel();
	~ServerLevel();

	void Update(const float aDeltaTime) override;

	void ReceiveNetworkMessage(const NetMessageRequestConnect& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageLevelLoaded& aMessage, const sockaddr_in& aSenderAddress) override;
	//void ReceiveMessage(const PostMasterNetAddPlayerMessage& aMessage);
	//void ReceiveMessage(const PostMasterNetLevelLoadedMessage& aMessage);

private:
	unsigned int myEntityIDCount;
	CU::GrowingArray<unsigned int> myLoadedClients;
};

