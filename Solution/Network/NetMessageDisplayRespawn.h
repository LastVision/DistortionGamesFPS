#pragma once
#include "NetImportantMessage.h"

class NetMessageDisplayRespawn : public NetImportantMessage
{
public:
	NetMessageDisplayRespawn();
	NetMessageDisplayRespawn(const CU::Vector3<float>& aPosition, bool aDisplayRespawn, int aCurrentValue, int aMaxValue);

	CU::Vector3<float> myPosition;
	bool myDisplayRespawn;
	int myCurrentValue;
	int myMaxValue;

private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;

};

inline NetMessageDisplayRespawn::NetMessageDisplayRespawn()
	: NetImportantMessage(eNetMessageType::DISPLAY_RESPAWN)
{
}

inline NetMessageDisplayRespawn::NetMessageDisplayRespawn(const CU::Vector3<float>& aPosition, bool aDisplayRespawn, int aCurrentValue, int aMaxValue)
	: NetImportantMessage(eNetMessageType::DISPLAY_RESPAWN)
	, myPosition(aPosition)
	, myDisplayRespawn(aDisplayRespawn)
	, myCurrentValue(aCurrentValue)
	, myMaxValue(aMaxValue)
{
}

inline void NetMessageDisplayRespawn::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myPosition);
	SERIALIZE(aStream, myDisplayRespawn);
	SERIALIZE(aStream, myCurrentValue);
	SERIALIZE(aStream, myMaxValue);
}

inline void NetMessageDisplayRespawn::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myPosition);
	DESERIALIZE(aStream, myDisplayRespawn);
	DESERIALIZE(aStream, myCurrentValue);
	DESERIALIZE(aStream, myMaxValue);
}