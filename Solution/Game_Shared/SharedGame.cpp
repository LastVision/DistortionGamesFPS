#include "stdafx.h"
#include "SharedGame.h"
#include <TimerManager.h>

SharedGame::SharedGame()
	: myTimerManager(new CU::TimerManager())
{
}

SharedGame::~SharedGame()
{
	SAFE_DELETE(myTimerManager);
}

void SharedGame::ManualDestroy()
{

}
