#pragma once
#include "NetImportantMessage.h"
#include "NetworkMessageTypes.h"

class NetMessagePressE : public NetImportantMessage
{
public:
	NetMessagePressE();
	NetMessagePressE(int aGID);
	~NetMessagePressE();

	int myGID;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessagePressE::NetMessagePressE()
	: NetImportantMessage(eNetMessageType::PRESS_E)
{
}

inline NetMessagePressE::NetMessagePressE(int aGID)
	: NetImportantMessage(eNetMessageType::PRESS_E)
	, myGID(aGID)
{
}

inline NetMessagePressE::~NetMessagePressE()
{
}

inline void NetMessagePressE::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myGID);
}

inline void NetMessagePressE::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myGID);
}