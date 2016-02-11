#pragma once
#include "Message.h"

enum class eOnClickEvent
{
	GAME_START,
	GAME_START_EASY,
	GAME_START_NORMAL,
	GAME_START_HARD,
	GAME_LEVEL_SELECT,
	GAME_CREDIT,
	GAME_QUIT,
	GAME_RESTART,
	GAME_HELP,
	GAME_WIN,
	GAME_LOSE,
	SPAWN_LOCKED_UNIT,
	SPAWN_UNIT,
	UNIT_ACTION_MOVE,
	UNIT_ACTION_ATTACK,
	UNIT_ACTION_STOP,
	UNIT_ACTION_ATTACK_MOVE,
	UNIT_ACTION_STAND_GROUND,
	PLACE_TOTEM,
	ENRAGE,
	SELECT_CONTROL_GROUP,
	UPGRADE_UNIT,
	RESUME_GAME,
	ABORT_QUEUE,
	INCREASEVOLUME,
	LOWERVOLUME,
	INCREASEMUSIC,
	LOWERMUSIC,
	INCREASEVOICE,
	LOWERVOICE,
	OPTIONS_MENU,
	TOGGLE_SHADOWS,
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