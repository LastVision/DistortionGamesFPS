#pragma once
#include "NetImportantMessage.h"

class NetMessageRayCastRequest : public NetImportantMessage
{
public:
	NetMessageRayCastRequest();
	NetMessageRayCastRequest(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aDirection, int aRayCastType
		, float aMaxLength, int aGID);

	CU::Vector3<float> myPosition;
	CU::Vector3<float> myDirection;

	int myRayCastType;
	float myMaxLength;
	unsigned int myGID;

private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;

};

inline NetMessageRayCastRequest::NetMessageRayCastRequest()
	: NetImportantMessage(eNetMessageType::RAY_CAST_REQUEST)
{
}

inline NetMessageRayCastRequest::NetMessageRayCastRequest(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aDirection, int aRayCastType
	, float aMaxLength, int aGID)
	: NetImportantMessage(eNetMessageType::RAY_CAST_REQUEST)
	, myPosition(aPosition)
	, myDirection(aDirection)
	, myRayCastType(aRayCastType)
	, myMaxLength(aMaxLength)
	, myGID(aGID)
{
}

inline void NetMessageRayCastRequest::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myPosition);
	SERIALIZE(aStream, myDirection);
	SERIALIZE(aStream, myRayCastType);
	SERIALIZE(aStream, myMaxLength);
	SERIALIZE(aStream, myGID);
}

inline void NetMessageRayCastRequest::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myPosition);
	DESERIALIZE(aStream, myDirection);
	DESERIALIZE(aStream, myRayCastType);
	DESERIALIZE(aStream, myMaxLength);
	DESERIALIZE(aStream, myGID);
}