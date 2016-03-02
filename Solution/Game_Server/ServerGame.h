#pragma once
#include <SharedGame.h>
class ServerGame : public SharedGame
{
public:
	ServerGame();
	~ServerGame();

	bool Update() override;
private:

};

