#include "stdafx.h"
#include "NetMessageImportantReply.h"
#include "NetworkMessageTypes.h"

NetMessageImportantReply::NetMessageImportantReply(const unsigned int aImportantID)
	: NetMessage(eNetMessageType::IMPORTANT_REPLY)
	, myImportantID(aImportantID)
{
}

NetMessageImportantReply::NetMessageImportantReply()
	: NetMessage(eNetMessageType::IMPORTANT_REPLY)
{
}

NetMessageImportantReply::~NetMessageImportantReply()
{
}

void NetMessageImportantReply::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myImportantID);
}

void NetMessageImportantReply::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myImportantID);
}
