#pragma once
#include "NetImportantMessage.h"
#include <string>
class NetMessageOnJoin : public NetImportantMessage
{
public:
	NetMessageOnJoin();
	NetMessageOnJoin(const std::string& aName, unsigned int aSenderID);
	~NetMessageOnJoin();

	std::string myName;

protected:
	
	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

