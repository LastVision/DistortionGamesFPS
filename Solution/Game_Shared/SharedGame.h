#pragma once
class SharedGame
{
public:
	SharedGame();
	virtual ~SharedGame();
	
	virtual void ManualDestroy();

	virtual bool Update() = 0;

protected:
};

