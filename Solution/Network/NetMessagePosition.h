#pragma once
#include "NetMessage.h"
#include <Vector3.h>
class NetMessagePosition : public NetMessage
{
public:
	NetMessagePosition();
	NetMessagePosition(const CU::Vector3<float>& aPos, float aRotationY, unsigned int aNetworkID);

	~NetMessagePosition();


	CU::Vector3<float> myPosition;
	float myRotationY;
	unsigned int myNetworkID;
protected:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

