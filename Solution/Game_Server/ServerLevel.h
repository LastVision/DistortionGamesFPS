#pragma once
#include <SharedLevel.h>
class ServerLevel : public SharedLevel
{
public:
	ServerLevel();
	~ServerLevel();

	void Update(const float aDeltaTime) override;
private:
	float myTestTimer;
};

