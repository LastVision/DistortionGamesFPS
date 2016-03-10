#include "stdafx.h"
#include "NetImportantMessage.h"


NetImportantMessage::NetImportantMessage()
{
}


NetImportantMessage::~NetImportantMessage()
{
}

bool NetImportantMessage::GetIsImportant()
{
	return true;
}

const unsigned int NetImportantMessage::GetImportantID() const
{
	return myImportantID;
}

void NetImportantMessage::SetImportantID(unsigned int anImportantID)
{
	myImportantID = anImportantID;
}

void NetImportantMessage::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myImportantID);
}

void NetImportantMessage::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myImportantID);
}
