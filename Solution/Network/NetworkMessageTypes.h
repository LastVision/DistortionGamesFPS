#pragma once
#include <Vector3.h>
#include <string>
enum class eNetMessageType
{
	NONE,
	IMPORTANT_REPLY,
	
	CONNECT_REPLY,
	ON_CONNECT,
	ON_JOIN,
	ON_DISCONNECT,
	REQUEST_LEVEL,
	REQUEST_START_LEVEL,
	LOAD_LEVEL,
	LEVEL_LOADED,
	LEVEL_COMPLETE,
	ALL_CLIENTS_COMPLETE,

	PING_REQUEST,
	PING_REPLY,
	
	POSITION,
	
	ON_HIT,
	ON_DEATH,
	ENEMY_SHOOTING,

	SET_ACTIVE,
	ENTITY_STATE,

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
	int apa = 4;
	apa++;
}