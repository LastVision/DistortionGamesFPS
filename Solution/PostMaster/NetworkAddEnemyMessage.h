#pragma once
#include "Message.h"
#include <Vector3.h>
struct NetworkAddEnemyMessage : public Message
{
	NetworkAddEnemyMessage(const CU::Vector3<float>& aPosition, unsigned int aNetworkID, const sockaddr_in& anAddress);
	NetworkAddEnemyMessage(const CU::Vector3<float>& aPosition, unsigned int aNetworkID);

	CU::Vector3<float> myPosition;
	unsigned int myNetworkID;
	sockaddr_in myAddress;
};

inline NetworkAddEnemyMessage::NetworkAddEnemyMessage(const CU::Vector3<float>& aPosition, unsigned int aNetworkID, const sockaddr_in& anAddress)
	: Message(eMessageType::NETWORK_ADD_ENEMY)
	, myPosition(aPosition)
	, myAddress(anAddress)
	, myNetworkID(aNetworkID)
{
}

inline NetworkAddEnemyMessage::NetworkAddEnemyMessage(const CU::Vector3<float>& aPositon, unsigned int aNetworkID)
	: Message(eMessageType::NETWORK_ADD_ENEMY)
	, myPosition(aPositon)
	, myNetworkID(aNetworkID)
{
}
