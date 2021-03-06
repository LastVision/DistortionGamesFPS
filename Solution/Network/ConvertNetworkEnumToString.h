#pragma once
#include "NetworkMessageTypes.h"
#include <DL_Debug.h>
#include <CommonHelper.h>

static std::string ConvertNetworkEnumToString(eNetMessageType aType)
{
	switch (aType)
	{
	case eNetMessageType::NONE:
		return "NONE";
	case eNetMessageType::IMPORTANT_REPLY:
		return "IMPORTANT REPLY";
	case eNetMessageType::KILL_SERVER:
		return "KILL SERVER";
	case eNetMessageType::CONNECT_REPLY:
		return "CONNECT REPLY";
	case eNetMessageType::ON_CONNECT:
		return "ON CONNECT";
	case eNetMessageType::ON_JOIN:
		return "ON JOIN";
	case eNetMessageType::ON_DISCONNECT:
		return "ON DISCONNECT";
	case eNetMessageType::SET_LEVEL:
		return "SET LEVEL";
	case eNetMessageType::REQUEST_LEVEL:
		return "REQUEST LEVEL";
	case eNetMessageType::REQUEST_START_LEVEL:
		return "REQUEST START GAME";
	case eNetMessageType::LOAD_LEVEL:
		return "START GAME";
	case eNetMessageType::LEVEL_LOADED:
		return "LEVEL LOADED";
	case eNetMessageType::LEVEL_COMPLETE:
		return "LEVEL COMPLETE";
	case eNetMessageType::ALL_CLIENTS_COMPLETE:
		return "ALL CLIENTS COMPLETE";
	case eNetMessageType::PING_REQUEST:
		return "PING REQUEST";
	case eNetMessageType::PING_REPLY:
		return "PING REPLY";
	case eNetMessageType::POSITION:
		return "POSITION";
	case eNetMessageType::ON_HIT:
		return "ON HIT";
	case eNetMessageType::ON_DEATH:
		return "ON DEATH";
	case eNetMessageType::SET_ACTIVE:
		return "SET ACTIVE";
	case eNetMessageType::ENTITY_STATE:
		return "ENTITY STATE";
	case eNetMessageType::ENEMY_SHOOTING:
		return "ENEMY_SHOOTING";
	case eNetMessageType::HEALTH:
		return "HEALTH MESSAGE TO PLAYER";
	case eNetMessageType::HEALTH_PACK:
		return "HEALTHPACK MESSAGE TO SERVER PLAYER TOOK";
	case eNetMessageType::SHOOT_GRENADE:
		return "CLIENT SHOOT GRENADE TO SERVER";
	case eNetMessageType::EXPLOSION:
		return "EXPLOSION TO CLIENT";
	case eNetMessageType::ACTIVATE_SPAWNPOINT:
		return "ACTIVATE_SPAWNPOINT ON SERVER";
	case eNetMessageType::ACTIVATE_UNIT:
		return "ACTIVATE_UNIT ON CLIENT";
	case eNetMessageType::TEXT:
		return "TEXT TO CLIENT";
	case eNetMessageType::DISPLAY_MARKER:
		return "DISPLAY MARKER ON CLIENT";
	case eNetMessageType::DISPLAY_RESPAWN:
		return "DISPLAY RESPAWN ON CLIENT";
	case eNetMessageType::SERVER_REPLY:
		return "SERVER REPLY";
	case eNetMessageType::SERVER_REQUEST:
		return "SERVER REQUEST";
	case eNetMessageType::START_LOBBY:
		return "START LOBBY";
	case eNetMessageType::RAY_CAST_REQUEST:
		return "CLIENT WANTS TO RAY CAST";
	case eNetMessageType::PRESS_E:
		return "CLIENT WANTS TO PRESS E";
	case eNetMessageType::PRESS_E_TEXT:
		return "SERVER SEND PRESS E TEXT";
	case eNetMessageType::_COUNT:
		DL_ASSERT("_COUNT not used as regular enum value");
	default:
		DL_ASSERT(CU::Concatenate("Unknown eNetMessageType: %i", int(aType)));
	}
	return "NOT VALID";
}