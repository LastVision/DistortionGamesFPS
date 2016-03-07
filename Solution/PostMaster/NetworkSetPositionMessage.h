#pragma once
#include "Message.h"
#include <Vector.h>
struct NetworkSetPositionMessage : public Message
{
	NetworkSetPositionMessage(const CU::Vector3<float>& aPosition, unsigned int aNetworkID);


	CU::Vector3<float> myPosition;
	unsigned int myNetworkID;
};

inline NetworkSetPositionMessage::NetworkSetPositionMessage(const CU::Vector3<float>& aPosition, unsigned int aNetworkID)
	: Message(eMessageType::NETWORK_SET_POSITION)
	, myPosition(aPosition)
	, myNetworkID(aNetworkID)
{
}
