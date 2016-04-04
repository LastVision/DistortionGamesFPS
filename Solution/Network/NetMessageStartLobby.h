#pragma once
#include "NetImportantMessage.h"

class NetMessageStartLobby : public NetImportantMessage
{
public:
	enum class eType : unsigned char
	{
		SINGLEPLAYER,
		MULTIPLAYER,
	};

	NetMessageStartLobby(eType aType);
	NetMessageStartLobby();
	~NetMessageStartLobby();

	eType myType;

private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageStartLobby::NetMessageStartLobby(eType aType)
	: NetImportantMessage(eNetMessageType::START_LOBBY)
	, myType(aType)
{
}

inline NetMessageStartLobby::NetMessageStartLobby()
	: NetImportantMessage(eNetMessageType::START_LOBBY)
{
}

inline NetMessageStartLobby::~NetMessageStartLobby()
{
}

inline void NetMessageStartLobby::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);

	SERIALIZE(aStream, myType);
}

inline void NetMessageStartLobby::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

	DESERIALIZE(aStream, myType);
}