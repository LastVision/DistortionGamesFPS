#include "stdafx.h"
#include "NetMessageConnectMessage.h"
#include "NetworkMessageTypes.h"

NetMessageConnectMessage::NetMessageConnectMessage()
{
}


NetMessageConnectMessage::~NetMessageConnectMessage()
{
}


void NetMessageConnectMessage::Init(const std::string& aName, short aServerID)
{
	myClientsOnServer.Init(8);
	myID = int(eNetMessageType::ON_CONNECT);
	myName = aName;
	myServerID = aServerID;
}

void NetMessageConnectMessage::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myName);
	SERIALIZE(aStream, myServerID);
	SERIALIZE(aStream, myClientsOnServer.Size());
	for (unsigned short i : myClientsOnServer)
	{
		SERIALIZE(aStream, i);
	}
}

void NetMessageConnectMessage::DoDeSerialize(StreamType& aStream)
{
	myClientsOnServer.Init(8);
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myName);
	DESERIALIZE(aStream, myServerID);
	int size = 0;
	DESERIALIZE(aStream, size);
	for (int i = 0; i < size; ++i)
	{
		unsigned short object;
		DESERIALIZE(aStream, object);
		myClientsOnServer.Add(object);
	}
}
