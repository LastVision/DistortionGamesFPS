#include "stdafx.h"
#include "NetMessageSetActive.h"

void NetMessageSetActive::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);

	SERIALIZE(aStream, myShouldActivate);
	SERIALIZE(aStream, myGID);
}

void NetMessageSetActive::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

	DESERIALIZE(aStream, myShouldActivate);
	DESERIALIZE(aStream, myGID);
}

