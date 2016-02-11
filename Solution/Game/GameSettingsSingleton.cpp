#include "stdafx.h"
#include "GameSettingsSingleton.h"
#include "GameDefines.h"

GameSettingsSingleton* GameSettingsSingleton::myInstance = nullptr;
GameSettingsSingleton* GameSettingsSingleton::GetInstance()
{
	if (myInstance == nullptr)
	{
		myInstance = new GameSettingsSingleton();
	}

	return myInstance;
}

void GameSettingsSingleton::Destroy()
{
	SAFE_DELETE(myInstance);
}


GameSettingsSingleton::GameSettingsSingleton()
	: myShouldUseShadows(true)
{
}


GameSettingsSingleton::~GameSettingsSingleton()
{
}
