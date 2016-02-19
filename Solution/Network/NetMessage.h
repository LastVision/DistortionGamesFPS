#pragma once
#include "SerializeHelper.h"
#include <stdint.h>
#define SERIALIZE(aStream, aType) serialize(aType,aStream);
#define DESERIALIZE(aStream, aType) aType = deserialize<decltype(aType)>(aStream)
class CNetMessage
{
public:
	CNetMessage();
	virtual ~CNetMessage();

	void PackMessage();
	void UnPackMessage(char* aMessage, int aSize);


	StreamType myStream;
	uint32_t myTimeStamp;
	uint16_t mySenderID;
	uint16_t myTargetID;
protected:

	virtual void DoSerialize(StreamType& aStream);
	virtual void DoDeSerialize(StreamType& aStream);

	uint8_t myID;



};

