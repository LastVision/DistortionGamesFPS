#pragma once
#include "NetImportantMessage.h"

class NetMessageEnemyShooting : public NetImportantMessage
{
public:
	NetMessageEnemyShooting();
	NetMessageEnemyShooting(int aGID);
	~NetMessageEnemyShooting();

	int myGID;
private:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;


};

inline NetMessageEnemyShooting::NetMessageEnemyShooting(int aGID)
	: NetImportantMessage(eNetMessageType::ENEMY_SHOOTING)
	, myGID(aGID)
{
}

inline NetMessageEnemyShooting::NetMessageEnemyShooting()
	: NetImportantMessage(eNetMessageType::ENEMY_SHOOTING)
{
}

inline NetMessageEnemyShooting::~NetMessageEnemyShooting()
{
}

inline void NetMessageEnemyShooting::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myGID);
}

inline void NetMessageEnemyShooting::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myGID);
}
