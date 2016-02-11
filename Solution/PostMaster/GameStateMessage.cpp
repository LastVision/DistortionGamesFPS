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

GameStateMessage::GameStateMessage(eGameState aGameState, const std::string& aFilePath)
	: myGameState(aGameState)
	, myFilePath(aFilePath)
	, myID(-1)
	, myTime(0.f)
	, myMouseIsLocked(false)
	, mySecondID(-1)
	, myText(nullptr)
	, Message(eMessageType::GAME_STATE)
{
}

GameStateMessage::GameStateMessage(eGameState aGameState, const std::string& aFilePath, const int& aID)
	: myGameState(aGameState)
	, myFilePath(aFilePath)
	, myID(aID)
	, myTime(0.f)
	, myMouseIsLocked(false)
	, mySecondID(-1)
	, myText(nullptr)
	, Message(eMessageType::GAME_STATE)
{
}

GameStateMessage::GameStateMessage(eGameState aGameState, const float& aTime, Prism::Text* aText)
	: myGameState(aGameState)
	, myTime(aTime)
	, myID(-1)
	, mySecondID(-1)
	, myMouseIsLocked(false)
	, myText(aText)
	, Message(eMessageType::GAME_STATE)
{
}

GameStateMessage::GameStateMessage(eGameState aGameState, const int& anID)
	: myGameState(aGameState)
	, myID(anID)
	, myTime(0.f)
	, myMouseIsLocked(false)
	, mySecondID(-1)
	, myText(nullptr)
	, Message(eMessageType::GAME_STATE)
{
}

GameStateMessage::GameStateMessage(eGameState aGameState, const int& anID, const int& anSecondID)
	: myGameState(aGameState)
	, myID(anID)
	, myTime(0.f)
	, myMouseIsLocked(false)
	, mySecondID(anSecondID)
	, myText(nullptr)
	, Message(eMessageType::GAME_STATE)
{
}

GameStateMessage::GameStateMessage(eGameState aGameState, const bool& anIsMouseLocked)
	: myGameState(aGameState)
	, myMouseIsLocked(anIsMouseLocked)
	, myID(-1)
	, mySecondID(-1)
	, myText(nullptr)
	, Message(eMessageType::GAME_STATE)
{
}
