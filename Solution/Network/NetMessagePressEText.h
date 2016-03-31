#pragma once
#include "NetImportantMessage.h"
#include "NetworkMessageTypes.h"

class NetMessagePressEText : public NetImportantMessage
{
public:
	NetMessagePressEText();
	NetMessagePressEText(int aGID, const CU::Vector3<float>& aPosition, bool aShouldAdd);
	~NetMessagePressEText();

	int myGID;
	CU::Vector3<float> myPosition;
	bool myShouldAdd;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessagePressEText::NetMessagePressEText()
	: NetImportantMessage(eNetMessageType::PRESS_E_TEXT)
{
}

inline NetMessagePressEText::NetMessagePressEText(int aGID, const CU::Vector3<float>& aPosition, bool aShouldAdd)
	: NetImportantMessage(eNetMessageType::PRESS_E_TEXT)
	, myGID(aGID)
	, myPosition(aPosition)
	, myShouldAdd(aShouldAdd)
{
}

inline NetMessagePressEText::~NetMessagePressEText()
{
}

inline void NetMessagePressEText::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myGID);
	SERIALIZE(aStream, myPosition);
	SERIALIZE(aStream, myShouldAdd);
}

inline void NetMessagePressEText::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myGID);
	DESERIALIZE(aStream, myPosition);
	DESERIALIZE(aStream, myShouldAdd);
}