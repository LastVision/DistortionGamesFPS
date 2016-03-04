#include "stdafx.h"
#include "NetMessage.h"



NetMessage::NetMessage()
{
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

bool NetMessage::GetIsImportant()
{
	return false;
}

void NetMessage::DoSerialize(StreamType& aStream)
{
	SERIALIZE(aStream, myID);
	SERIALIZE(aStream, myTimeStamp);
	SERIALIZE(aStream, mySenderID);
	SERIALIZE(aStream, myTargetID);

}

void NetMessage::DoDeSerialize(StreamType& aStream)
{
	DESERIALIZE(aStream, myID);
	DESERIALIZE(aStream, myTimeStamp);
	DESERIALIZE(aStream, mySenderID);
	DESERIALIZE(aStream, myTargetID);
}
