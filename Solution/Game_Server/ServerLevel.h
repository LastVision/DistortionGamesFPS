#pragma once
#include <SharedLevel.h>

class ServerLevel : public SharedLevel
{
public:
	ServerLevel();
	~ServerLevel();

	void Update(const float aDeltaTime) override;
	void ReceiveMessage(const NetworkAddPlayerMessage& aMessage);
private:
	unsigned int myEntityIDCount;
	
};

