#pragma once
#include <string>
struct SelectionComponentData
{
	bool myExistsInEntity = false;
	std::string mySelectedPath;
	std::string myHoveredPath;
	std::string myEffectPath;
	float myScale;
};