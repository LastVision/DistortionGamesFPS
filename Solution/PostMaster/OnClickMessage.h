#pragma once
#include "Message.h"

enum class eOnClickEvent
{
	CONNECT,
	START_GAME,
	START_SINGLEPLAYER,
	HOST_MULTIPLAYER,
	JOIN_MULTIPLAYER,
	CREDITS,
	GAME_QUIT,
	RESUME_GAME,
	OPTIONS,
	HELP,
};

struct OnClickMessage : public Message
{
	OnClickMessage(eOnClickEvent anEvent);
	OnClickMessage(eOnClickEvent anEvent, int anID);

	const eOnClickEvent myEvent;
	const int myID;
};

inline OnClickMessage::OnClickMessage(eOnClickEvent anEvent)
	: Message(eMessageType::ON_CLICK)
	, myEvent(anEvent)
	, myID(-1)
{
}

inline OnClickMessage::OnClickMessage(eOnClickEvent anEvent, int anID)
	: Message(eMessageType::ON_CLICK)
	, myEvent(anEvent)
	, myID(anID)
{

}