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
	case eNetMessageType::CONNECT_REPLY:
		return "CONNECT REPLY";
	case eNetMessageType::ON_CONNECT:
		return "ON CONNECT";
	case eNetMessageType::ON_JOIN:
		return "ON JOIN";
	case eNetMessageType::ON_DISCONNECT:
		return "ON DISCONNECT";
	case eNetMessageType::REQUEST_LEVEL:
		return "REQUEST LEVEL";
	case eNetMessageType::REQUEST_START_GAME:
		return "REQUEST START GAME";
	case eNetMessageType::START_GAME:
		return "START GAME";
	case eNetMessageType::LEVEL_LOADED:
		return "LEVEL LOADED";
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
	case eNetMessageType::_COUNT:
		DL_ASSERT("_COUNT not used as regular enum value");
	default:
		DL_ASSERT(CU::Concatenate("Unknown eNetMessageType: %i", int(aType)));
	}
	return "NOT VALID";
}