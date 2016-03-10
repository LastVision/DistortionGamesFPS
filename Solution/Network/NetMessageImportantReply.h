#pragma once
#include "NetMessage.h"
class NetMessageImportantReply :
	public NetMessage
{
public:
	NetMessageImportantReply(const unsigned int aImportantID);
	NetMessageImportantReply();
	~NetMessageImportantReply();

	unsigned int myImportantID;
private:
	void DoSerialize(StreamType& aStream);
	void DoDeSerialize(StreamType& aStream);
};

