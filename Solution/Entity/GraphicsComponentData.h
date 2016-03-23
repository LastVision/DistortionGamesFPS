#pragma once
#include <GameEnum.h>
#include <string>

struct GraphicsComponentData
{
	bool myExistsInEntity = false;
	std::string myModelPath;
	std::string myEffectPath;
	eObjectRoomType myRoomType;
};