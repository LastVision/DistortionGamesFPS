#include "stdafx.h"
#include "NetMessage.h"



NetMessage::NetMessage(eNetMessageType aID)
{
	myID = static_cast<uint8_t>(aID);
	myGameID = static_cast<uint16_t>('DG');
}


NetMessage::~NetMessage()
{
}

void NetMessage::PackMessage()
{
	DoSerialize(myStream);
}

void NetMessage::UnPackMessage(char* aMessage, int aSize)
{
	if (aSize > 0)
	{
		for (int i = 0; i < aSize; ++i)
		{
			myStream.push_back(aMessage[i]);
		}
		DoDeSerialize(myStream);
	}
}

bool NetMessage::GetIsImportant() const
{
	return false;
}

const unsigned int NetMessage::GetImportantID() const
{
	return 0;
}

void NetMessage::DoSerialize(StreamType& aStream)
{
	SERIALIZE(aStream, myGameID);
	SERIALIZE(aStream, myID);
	SERIALIZE(aStream, myTimeStamp);
	SERIALIZE(aStream, mySenderID);
	SERIALIZE(aStream, myTargetID);

}

void NetMessage::DoDeSerialize(StreamType& aStream)
{
	DESERIALIZE(aStream, myGameID);
	DESERIALIZE(aStream, myID);
	DESERIALIZE(aStream, myTimeStamp);
	DESERIALIZE(aStream, mySenderID);
	DESERIALIZE(aStream, myTargetID);
}
