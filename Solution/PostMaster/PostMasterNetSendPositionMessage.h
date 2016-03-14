#pragma once
#include "Message.h"
#include <Vector.h>
struct PostMasterNetSendPositionMessage : public Message
{
	PostMasterNetSendPositionMessage(const CU::Vector3<float>& aPosition, float aRotationY, unsigned int aGID);


	CU::Vector3<float> myPosition;
	float myRotationY;
	unsigned int myGID;
};

inline PostMasterNetSendPositionMessage::PostMasterNetSendPositionMessage(const CU::Vector3<float>& aPosition, float aRotationY, unsigned int aGID)
	: Message(eMessageType::NETWORK_SEND_POSITION)
	, myPosition(aPosition)
	, myRotationY(aRotationY)
	, myGID(aGID)
{
}
