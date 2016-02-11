#pragma once
#include <string>
struct AnimationLoadData
{
	eEntityState myEntityState;
	std::string myAnimationPath;
	bool myLoopFlag;
	bool myResetTimeOnRestart;
};

struct AnimationComponentData
{
	bool myExistsInEntity = false;
	std::string myModelPath;
	std::string myEffectPath;
	CU::StaticArray<AnimationLoadData, static_cast<int>(eEntityState::_COUNT)> myAnimations;
};