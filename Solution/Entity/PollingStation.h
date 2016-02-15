#pragma once

class Entity;
class PollingStation
{
public:
	static PollingStation* GetInstance();
	static void Destroy();


private:
	PollingStation();
	~PollingStation();

	static PollingStation* myInstance;
};


