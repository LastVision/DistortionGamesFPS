#pragma once
#include "Message.h"
#include <Vector.h>
struct NetworkSendPositionMessage : public Message
{
	NetworkSendPositionMessage(const CU::Vector3<float>& aPosition, float aRotationY, unsigned int aNetworkID);


	CU::Vector3<float> myPosition;
	float myRotationY;
	unsigned int myNetworkID;
};

inline NetworkSendPositionMessage::NetworkSendPositionMessage(const CU::Vector3<float>& aPosition, float aRotationY, unsigned int aNetworkID)
	: Message(eMessageType::NETWORK_SEND_POSITION)
	, myPosition(aPosition)
	, myRotationY(aRotationY)
	, myNetworkID(aNetworkID)
{
}
