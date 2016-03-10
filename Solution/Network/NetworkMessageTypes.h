#pragma once
#include <Vector3.h>
enum class eNetMessageType
{
	NONE,
	IMPORTANT_REPLY,
	
	ON_CONNECT,
	ON_JOIN,
	ON_DISCONNECT,

	PING_REQUEST,
	PING_REPLY,
	
	POSITION,
	ADD_ENEMY,
	
	PLAYER_ON_HIT,
	ENEMY_ON_HIT,
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
	short myID;
	bool myIsConnected;
};

struct OtherClients
{
	OtherClients();
	OtherClients(unsigned short anID);
	unsigned short myID;
	CU::Vector3<float> myPosition;
};

inline OtherClients::OtherClients()
{
}

inline OtherClients::OtherClients(unsigned short anID)
{
	myID = anID;
}