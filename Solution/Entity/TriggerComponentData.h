#pragma once

struct TriggerComponentData
{
	bool myExistsInEntity = false;
	bool myIsActiveFromStart = true;
	bool myIsOneTime;
	bool myIsClientSide;
	bool myShowMarker;
	bool myIsPressable;
	int myTriggerType;
	int myValue;
	CU::Vector3<float> myPosition;
};