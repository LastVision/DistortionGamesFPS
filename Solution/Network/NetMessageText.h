#pragma once
#include "NetImportantMessage.h"

class NetMessageText : public NetImportantMessage
{
public:
	NetMessageText(std::string aText);
	NetMessageText();
	~NetMessageText(){};

	std::string myText;

private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageText::NetMessageText(std::string aText)
	: NetImportantMessage(eNetMessageType::TEXT)
	, myText(aText)
{
}

inline NetMessageText::NetMessageText()
	: NetImportantMessage(eNetMessageType::TEXT)
{
}

inline void NetMessageText::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);

	SERIALIZE(aStream, myText);
}

inline void NetMessageText::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);

	DESERIALIZE(aStream, myText);
}
