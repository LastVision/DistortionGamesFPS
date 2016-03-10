#pragma once
#include "Message.h"
#include <string>

enum class eGameState
{
	LOAD_LEVEL
};

namespace Prism
{
	class Text;
}

struct GameStateMessage : public Message
{
public:
	GameStateMessage(eGameState aGameState);
	GameStateMessage(eGameState aGameState, const int& aID);
	eGameState myGameState;
	std::string myFilePath;
	int myID;
	int mySecondID;
	float myTime;
	bool myMouseIsLocked; 
	Prism::Text* myText;
};