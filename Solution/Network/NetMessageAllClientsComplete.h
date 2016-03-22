#pragma once
#include "NetImportantMessage.h"
#include "NetworkMessageTypes.h"

class NetMessageAllClientsComplete : public NetImportantMessage
{
public:
	enum eType : unsigned char
	{
		NONE,
		LEVEL_COMPLETE,
		LEVEL_LOAD,
	};
	NetMessageAllClientsComplete();
	NetMessageAllClientsComplete(eType aType);

	eType myType;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageAllClientsComplete::NetMessageAllClientsComplete(eType aType)
	: NetImportantMessage(eNetMessageType::ALL_CLIENTS_COMPLETE)
	, myType(aType)
{
}

inline NetMessageAllClientsComplete::NetMessageAllClientsComplete()
	: NetImportantMessage(eNetMessageType::ALL_CLIENTS_COMPLETE)
{
}

inline void NetMessageAllClientsComplete::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myType);
}

inline void NetMessageAllClientsComplete::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myType);
}



