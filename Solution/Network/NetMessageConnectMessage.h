#pragma once
#include "NetMessage.h"
class NetMessageConnectMessage : public NetMessage
{
public:
	NetMessageConnectMessage();
	~NetMessageConnectMessage() override;

	void Init(const std::string& aName, short aServerID);

	std::string myName;
	short myServerID;
protected:


	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;

};

