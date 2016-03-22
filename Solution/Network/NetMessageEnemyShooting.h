#pragma once
#include "NetImportantMessage.h"

class NetMessageEnemyShooting : public NetImportantMessage
{
public:
	NetMessageEnemyShooting();
	~NetMessageEnemyShooting();

private:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;

};

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
}

inline void NetMessageEnemyShooting::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
}
