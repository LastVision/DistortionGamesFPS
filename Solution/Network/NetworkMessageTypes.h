#pragma once
#include <Vector3.h>
enum class eNetMessageType
{
	NONE,
	
	ON_CONNECT,
	ON_JOIN,

	PING_REQUEST,
	PING_REPLY,
	
	POSITION,

	
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
	unsigned short myID;
	CU::Vector3<float> myPosition;
};