#pragma once
class SharedGame
{
public:
	SharedGame();
	virtual ~SharedGame();

	virtual bool Update() = 0;

protected:
};

