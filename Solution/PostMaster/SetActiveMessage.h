#pragma once
#include "Message.h"

struct SetActiveMessage : public Message
{
	SetActiveMessage(int aGID, bool aShouldActivate);
	const int myGID;
	const bool myShouldActivate;
}; 

inline SetActiveMessage::SetActiveMessage(int aGID, bool aShouldActivate)
	: Message(eMessageType::SET_ACTIVE)
	, myGID(aGID)
	, myShouldActivate(aShouldActivate)
{
}