#pragma once
#include "NetImportantMessage.h"
#include <GameEnum.h>

class NetMessageEntityState : public NetImportantMessage
{
public:
	NetMessageEntityState(eEntityState aState, unsigned int aGID);
	NetMessageEntityState();
	~NetMessageEntityState(){};

	unsigned char myEntityState;
	unsigned int myGID;

private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageEntityState::NetMessageEntityState(eEntityState aState, unsigned int aGID)
	: NetImportantMessage(eNetMessageType::ENTITY_STATE)
	, myEntityState(static_cast<unsigned char>(aState))
	, myGID(aGID)
{
}

inline NetMessageEntityState::NetMessageEntityState()
	: NetImportantMessage(eNetMessageType::ENTITY_STATE)
{
}

inline void NetMessageEntityState::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);

	SERIALIZE(aStream, myEntityState);
	SERIALIZE(aStream, myGID);
}

inline void NetMessageEntityState::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

	DESERIALIZE(aStream, myEntityState);
	DESERIALIZE(aStream, myGID);
}
