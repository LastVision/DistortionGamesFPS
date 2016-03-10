#pragma once

struct TriggerComponentData
{
	bool myExistsInEntity = false;
	bool myIsOneTime;
	int myTriggerType;
	int myValue;
	std::string myID;
};