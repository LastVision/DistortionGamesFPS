#include "stdafx.h"
#include "GameStateMessage.h"

GameStateMessage::GameStateMessage(eGameState aGameState)
	: myGameState(aGameState)
	, myText(nullptr)
	, myID(-1)
	, myTime(0.f)
	, myMouseIsLocked(false)
	, mySecondID(-1)
	, Message(eMessageType::GAME_STATE)
{
}

GameStateMessage::GameStateMessage(eGameState aGameState, const int& aID)
	: myGameState(aGameState)
	, myText(nullptr)
	, myID(aID)
	, myTime(0.f)
	, myMouseIsLocked(false)
	, mySecondID(-1)
	, Message(eMessageType::GAME_STATE)
{
}
