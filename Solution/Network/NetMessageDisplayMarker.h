#pragma once
#include "NetImportantMessage.h"

class NetMessageDisplayMarker : public NetImportantMessage
{
public:
	NetMessageDisplayMarker();
	NetMessageDisplayMarker(const CU::Vector3<float>& aPosition, bool aDisplayMarker);

	CU::Vector3<float> myPosition;
	bool myDisplayMarker;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;

};

inline NetMessageDisplayMarker::NetMessageDisplayMarker()
	: NetImportantMessage(eNetMessageType::ACTIVATE_UNIT)
{
}

inline NetMessageDisplayMarker::NetMessageDisplayMarker(const CU::Vector3<float>& aPosition, bool aDisplayMarker)
	: NetImportantMessage(eNetMessageType::ACTIVATE_UNIT)
	, myPosition(aPosition)
	, myDisplayMarker(aDisplayMarker)
{
}

inline void NetMessageDisplayMarker::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myPosition);
	SERIALIZE(aStream, myDisplayMarker);
}

inline void NetMessageDisplayMarker::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myPosition);
	DESERIALIZE(aStream, myDisplayMarker);
}