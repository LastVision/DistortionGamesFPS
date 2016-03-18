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

inline NetMessagePosition::NetMessagePosition(const CU::Vector3<float>& aPos, float aRotationY, unsigned int aGID)
	: NetMessage(eNetMessageType::POSITION)
	, myPosition(aPos)
	, myRotationY(aRotationY)
	, myGID(aGID)
{
}

inline NetMessagePosition::NetMessagePosition()
	: NetMessage(eNetMessageType::POSITION)
{
}

inline NetMessagePosition::~NetMessagePosition()
{
}

inline void NetMessagePosition::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myPosition);
	SERIALIZE(aStream, myRotationY);
	SERIALIZE(aStream, myGID);
}

inline void NetMessagePosition::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myPosition);
	DESERIALIZE(aStream, myRotationY);
	DESERIALIZE(aStream, myGID);
}
