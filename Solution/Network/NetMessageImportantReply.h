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

inline NetMessageImportantReply::NetMessageImportantReply(const unsigned int aImportantID)
	: NetMessage(eNetMessageType::IMPORTANT_REPLY)
	, myImportantID(aImportantID)
{
}

inline NetMessageImportantReply::NetMessageImportantReply()
	: NetMessage(eNetMessageType::IMPORTANT_REPLY)
{
}

inline NetMessageImportantReply::~NetMessageImportantReply()
{
}

inline void NetMessageImportantReply::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myImportantID);
}

inline void NetMessageImportantReply::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myImportantID);
}
