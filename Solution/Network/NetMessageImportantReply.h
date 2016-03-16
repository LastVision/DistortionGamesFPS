#pragma once
#include "NetMessage.h"
class NetMessageImportantReply :
	public NetMessage
{
public:
	NetMessageImportantReply(const unsigned int aImportantID);
	NetMessageImportantReply();
	~NetMessageImportantReply();

	void SetImportantID(unsigned int aID) override;

	unsigned int myImportantID;
private:
	void DoSerialize(StreamType& aStream);
	void DoDeSerialize(StreamType& aStream);
};

inline void NetMessageImportantReply::SetImportantID(unsigned int aID)
{
	myImportantID = aID;
}

