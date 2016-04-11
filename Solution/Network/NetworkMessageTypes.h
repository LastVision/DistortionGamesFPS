#pragma once
#include <Vector3.h>
#include <string>
enum class eNetMessageType
{
	NONE,
	IMPORTANT_REPLY,
	KILL_SERVER,
	
	START_LOBBY,
	CONNECT_REPLY,
	ON_CONNECT,
	ON_JOIN,
	ON_DISCONNECT,
	SET_LEVEL,
	REQUEST_LEVEL,
	REQUEST_START_LEVEL,
	LOAD_LEVEL,
	LEVEL_LOADED,
	LEVEL_COMPLETE,
	ALL_CLIENTS_COMPLETE,

	SERVER_REQUEST,
	SERVER_REPLY,

	PING_REQUEST,
	PING_REPLY,
	
	POSITION,
	
	HEALTH,
	HEALTH_PACK,
	ON_HIT,
	ON_DEATH,
	ENEMY_SHOOTING,
	SHOOT_GRENADE,
	EXPLOSION,
	TEXT,
	DISPLAY_MARKER,
	DISPLAY_RESPAWN,

	SET_ACTIVE,
	ENTITY_STATE,
	ACTIVATE_SPAWNPOINT,
	ACTIVATE_UNIT,

	RAY_CAST_REQUEST,
	RAY_CAST_RESULT,
	PRESS_E,
	PRESS_E_TEXT,

	_COUNT,
};

struct Buffer
{
	char myData[512];
	int myLength;
	sockaddr_in mySenderAddress;
};

struct Connection
{
	sockaddr_in myAddress;
	std::string myName;
	int myPingCount;
	unsigned int myID;
	bool myIsConnected;
};

struct OtherClients
{
	OtherClients();
	OtherClients(const std::string& aName, unsigned int anID);
	unsigned int myID;
	CU::Vector3<float> myPosition;
	std::string myName;
};

inline OtherClients::OtherClients()
{
}

inline OtherClients::OtherClients(const std::string& aName, unsigned int anID)
	: myName(aName)
	, myID(anID)
{
}