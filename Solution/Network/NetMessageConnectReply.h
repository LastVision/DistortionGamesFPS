#pragma once
#include "NetImportantMessage.h"

class NetMessageConnectReply : public NetImportantMessage
{
public:
	enum class eType : unsigned char
	{
		NONE,
		SUCCESS,
		FAIL,
	};

	NetMessageConnectReply(eType aType, unsigned int aGID = INT_MAX);
	NetMessageConnectReply();
	~NetMessageConnectReply();

	eType myType;
	unsigned int myGID;

private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageConnectReply::NetMessageConnectReply(eType aType, unsigned int aGID)
	: NetImportantMessage(eNetMessageType::CONNECT_REPLY)
	, myType(aType)
	, myGID(aGID)
{
}

inline NetMessageConnectReply::NetMessageConnectReply()
	: NetImportantMessage(eNetMessageType::CONNECT_REPLY)
{
}

inline NetMessageConnectReply::~NetMessageConnectReply()
{
}

inline void NetMessageConnectReply::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);

	SERIALIZE(aStream, myType);
	SERIALIZE(aStream, myGID);
}

inline void NetMessageConnectReply::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

	DESERIALIZE(aStream, myType);
	DESERIALIZE(aStream, myGID);
}