#pragma once
#include "NetImportantMessage.h"

class NetMessageDisplayRespawn : public NetImportantMessage
{
public:
	NetMessageDisplayRespawn();
	NetMessageDisplayRespawn(const CU::Vector3<float>& aPosition, bool aDisplayRespawn, int aCurrentValue, int aMaxValue, int aGID);

	CU::Vector3<float> myPosition;
	bool myDisplayRespawn;
	int myCurrentValue;
	int myMaxValue;
	int myGID;

private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;

};

inline NetMessageDisplayRespawn::NetMessageDisplayRespawn()
	: NetImportantMessage(eNetMessageType::DISPLAY_RESPAWN)
{
}

inline NetMessageDisplayRespawn::NetMessageDisplayRespawn(const CU::Vector3<float>& aPosition, bool aDisplayRespawn, int aCurrentValue, int aMaxValue, int aGID)
	: NetImportantMessage(eNetMessageType::DISPLAY_RESPAWN)
	, myPosition(aPosition)
	, myDisplayRespawn(aDisplayRespawn)
	, myCurrentValue(aCurrentValue)
	, myMaxValue(aMaxValue)
	, myGID(aGID)
{
}

inline void NetMessageDisplayRespawn::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myPosition);
	SERIALIZE(aStream, myDisplayRespawn);
	SERIALIZE(aStream, myCurrentValue);
	SERIALIZE(aStream, myMaxValue);
	SERIALIZE(aStream, myGID);
}

inline void NetMessageDisplayRespawn::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myPosition);
	DESERIALIZE(aStream, myDisplayRespawn);
	DESERIALIZE(aStream, myCurrentValue);
	DESERIALIZE(aStream, myMaxValue);
	DESERIALIZE(aStream, myGID);
}