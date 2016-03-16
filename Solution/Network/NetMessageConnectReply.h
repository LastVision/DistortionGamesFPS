#pragma once
#include "NetImportantMessage.h"

class NetMessageConnectReply : public NetImportantMessage
{
public:
	enum class eType : unsigned char
	{
		NONE,
		SUCCESS,
		FAIL,
	};

	NetMessageConnectReply(eType aType, unsigned int aGID = INT_MAX);
	NetMessageConnectReply();
	~NetMessageConnectReply();

	eType myType;
	unsigned int myGID;

private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

