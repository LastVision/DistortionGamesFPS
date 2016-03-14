#pragma once
#include "NetMessage.h"
#include <Vector3.h>
class NetMessagePosition : public NetMessage
{
public:
	NetMessagePosition();
	NetMessagePosition(const CU::Vector3<float>& aPos, float aRotationY, unsigned int aGID);

	~NetMessagePosition();


	CU::Vector3<float> myPosition;
	float myRotationY;
	unsigned int myGID;
protected:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

