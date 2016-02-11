#pragma once
#include "Message.h"
#include <string>

enum class eGameState
{
	LOAD_GAME,
	RELOAD_LEVEL,
	LOAD_MENU,
	COMPLETE_LEVEL,
	LOAD_NEXT_LEVEL,
	MOUSE_LOCK,
	EXIT_GAME,
	COMPLETE_GAME,
	CLICKABLE_STATE
};

namespace Prism
{
	class Text;
}

struct GameStateMessage : public Message
{
public:
	GameStateMessage(eGameState aGameState);
	GameStateMessage(eGameState aGameState, const std::string& aFilePath);
	GameStateMessage(eGameState aGameState, const std::string& aFilePath, const int& aID);
	GameStateMessage(eGameState aGameState, const float& aTime, Prism::Text* aText);
	GameStateMessage(eGameState aGameState, const int& anID);
	GameStateMessage(eGameState aGameState, const int& anID, const int& anSecondID);
	GameStateMessage(eGameState aGameState, const bool& anIsMouseLocked);

	eGameState myGameState;
	std::string myFilePath;
	int myID;
	int mySecondID;
	float myTime;
	bool myMouseIsLocked; 
	Prism::Text* myText;
};