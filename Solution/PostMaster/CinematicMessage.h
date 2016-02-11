#pragma once
#include "Message.h"
#include <string>

enum class eCinematicAction
{
	START,
	END
};

struct CinematicMessage : public Message
{
	CinematicMessage(int aCinematicIndex, eCinematicAction aAction);

	const int myCinematicIndex;
	const eCinematicAction myAction;
};

inline CinematicMessage::CinematicMessage(int aCinematicIndex, eCinematicAction aAction)
	: Message(eMessageType::CINEMATIC)
	, myCinematicIndex(aCinematicIndex)
	, myAction(aAction)
{
}