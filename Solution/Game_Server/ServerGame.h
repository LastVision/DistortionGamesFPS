#pragma once
#include "ServerStateStack.h"
#include <SharedGame.h>

class ServerGame : public SharedGame
{
public:
	ServerGame();
	~ServerGame();

	bool Init();
	bool Update() override;
private:
	ServerStateStack myStateStack;
};

