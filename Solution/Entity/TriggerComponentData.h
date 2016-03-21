#pragma once

struct TriggerComponentData
{
	bool myExistsInEntity = false;
	bool myIsActiveFromStart = true;
	bool myIsOneTime;
	bool myIsClientSide;
	int myTriggerType;
	int myValue;
};