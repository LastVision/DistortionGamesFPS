#pragma once
#include <GameEnum.h>
#include <string>

struct ActionEvent
{
	ActionEvent(){}
	eActionEventType myType;
	int myGID;
	float myTimeBeforeStarting;
	float myShowTextTime;
	std::string myText;
	bool myShow;
	CU::Vector3<float> myPosition;
};