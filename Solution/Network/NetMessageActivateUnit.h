#pragma once
#include "NetImportantMessage.h"


class NetMessageActivateUnit : public NetImportantMessage
{
public:
	NetMessageActivateUnit();
	NetMessageActivateUnit(unsigned int aGID, const CU::Vector3<float>& aPosition);

	unsigned int myGID;
	CU::Vector3<float> myPosition;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;

};

inline NetMessageActivateUnit::NetMessageActivateUnit()
	: NetImportantMessage(eNetMessageType::ACTIVATE_UNIT)
{
}

inline NetMessageActivateUnit::NetMessageActivateUnit(unsigned int aGID, const CU::Vector3<float>& aPosition)
	: NetImportantMessage(eNetMessageType::ACTIVATE_UNIT)
	, myGID(aGID)
	, myPosition(aPosition)
{
}

inline void NetMessageActivateUnit::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myGID);
	SERIALIZE(aStream, myPosition);
}

inline void NetMessageActivateUnit::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myGID);
	DESERIALIZE(aStream, myPosition);
}
