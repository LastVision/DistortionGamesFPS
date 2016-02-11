#pragma once
#include <string>
struct GraphicsComponentData
{
	bool myExistsInEntity = false;
	std::string myModelPath;
	std::string myEffectPath;
	bool myAlwaysRender;
};