#pragma once
#include "SerializeHelper.h"
#include <stdint.h>
#define SERIALIZE(aStream, aType) serialize(aType,aStream);
#define DESERIALIZE(aStream, aType) aType = deserialize<decltype(aType)>(aStream)
class NetMessage
{
public:
	NetMessage();
	virtual ~NetMessage();

	void PackMessage();
	void UnPackMessage(char* aMessage, int aSize);
	virtual bool GetIsImportant() const;
	virtual const unsigned int GetImportantID() const;

	StreamType myStream;
	uint32_t myTimeStamp;
	uint32_t mySenderID;
	uint32_t myTargetID;
	uint8_t myID;
protected:

	virtual void DoSerialize(StreamType& aStream);
	virtual void DoDeSerialize(StreamType& aStream);




};

