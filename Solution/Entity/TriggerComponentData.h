#pragma once

struct TriggerComponentData
{
	bool myExistsInEntity = false;
	bool myIsOneTime;
	bool myIsClientSide;
	int myTriggerType;
	int myValue;
};