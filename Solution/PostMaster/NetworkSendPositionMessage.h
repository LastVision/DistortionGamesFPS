#pragma once
#include "Message.h"
#include <Vector.h>
struct NetworkSendPositionMessage : public Message
{
	NetworkSendPositionMessage(const CU::Vector3<float>& aPosition, unsigned int aNetworkID);


	CU::Vector3<float> myPosition;
	unsigned int myNetworkID;
};

inline NetworkSendPositionMessage::NetworkSendPositionMessage(const CU::Vector3<float>& aPosition, unsigned int aNetworkID)
	: Message(eMessageType::NETWORK_SEND_POSITION)
	, myPosition(aPosition)
	, myNetworkID(aNetworkID)
{
}
