#pragma once
#include "NetImportantMessage.h"
class NetMessageDisconnect : public NetImportantMessage
{
public:
	NetMessageDisconnect(unsigned int aClientID);
	NetMessageDisconnect();
	~NetMessageDisconnect();

	unsigned int myClientID;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageDisconnect::NetMessageDisconnect(unsigned int aClientID)
	: NetImportantMessage(eNetMessageType::ON_DISCONNECT)
	, myClientID(aClientID)
{
}

inline NetMessageDisconnect::NetMessageDisconnect()
	: NetImportantMessage(eNetMessageType::ON_DISCONNECT)
{
}

inline NetMessageDisconnect::~NetMessageDisconnect()
{
}

inline void NetMessageDisconnect::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myClientID);
}

inline void NetMessageDisconnect::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myClientID);
}
