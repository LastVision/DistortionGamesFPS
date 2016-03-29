#pragma once

struct TriggerComponentData
{
	bool myExistsInEntity = false;
	bool myIsActiveFromStart = true;
	bool myIsOneTime;
	bool myIsClientSide;
	bool myShowMarker;
	int myTriggerType;
	int myValue;
	CU::Vector3<float> myPosition;
};