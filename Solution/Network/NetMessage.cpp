#include "stdafx.h"
#include "NetMessage.h"



CNetMessage::CNetMessage()
{
}


CNetMessage::~CNetMessage()
{
}

void CNetMessage::PackMessage()
{
	DoSerialize(myStream);
}

void CNetMessage::UnPackMessage(char* aMessage, int aSize)
{
	if (aSize > 0)
	{
		for (unsigned int i = 0; i < aSize; ++i)
		{
			myStream.push_back(aMessage[i]);
		}
		DoDeSerialize(myStream);
	}
}

void CNetMessage::DoSerialize(StreamType& aStream)
{
	SERIALIZE(aStream, myID);
	SERIALIZE(aStream, myTimeStamp);
	SERIALIZE(aStream, mySenderID);
	SERIALIZE(aStream, myTargetID);

}

void CNetMessage::DoDeSerialize(StreamType& aStream)
{
	DESERIALIZE(aStream, myID);
	DESERIALIZE(aStream, myTimeStamp);
	DESERIALIZE(aStream, mySenderID);
	DESERIALIZE(aStream, myTargetID);
}
