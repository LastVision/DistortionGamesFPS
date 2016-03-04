#pragma once
#include "Message.h"
#include <Vector3.h>
struct NetworkAddEnemyMessage : public Message
{
	NetworkAddEnemyMessage(const CU::Vector3<float>& aPosition, const sockaddr_in& anAddress);
	NetworkAddEnemyMessage(const CU::Vector3<float>& aPosition);

	CU::Vector3<float> myPosition;
	sockaddr_in myAddress;
};

inline NetworkAddEnemyMessage::NetworkAddEnemyMessage(const CU::Vector3<float>& aPosition, const sockaddr_in& anAddress)
	: Message(eMessageType::NETWORK_ADD_ENEMY)
	, myPosition(aPosition)
	, myAddress(anAddress)
{
}

inline NetworkAddEnemyMessage::NetworkAddEnemyMessage(const CU::Vector3<float>& aPositon)
	: Message(eMessageType::NETWORK_ADD_ENEMY)
	, myPosition(aPositon)
{
}
