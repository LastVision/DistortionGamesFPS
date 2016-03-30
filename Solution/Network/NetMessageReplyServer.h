#pragma once
#include "NetworkMessageTypes.h"
#include "NetMessage.h"

class NetMessageReplyServer : public NetMessage
{
public:
	NetMessageReplyServer();
	NetMessageReplyServer(const std::string& aServerName, const std::string& aIP);

	std::string myServerName;
	std::string myIP;
protected:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageReplyServer::NetMessageReplyServer()
	: NetMessage(eNetMessageType::SERVER_REPLY)
{
}

inline NetMessageReplyServer::NetMessageReplyServer(const std::string& aServerName, const std::string& aIP)
	: NetMessage(eNetMessageType::SERVER_REPLY)
	, myServerName(aServerName)
	, myIP(aIP)
{
}

inline void NetMessageReplyServer::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myServerName);
	SERIALIZE(aStream, myIP);
}

inline void NetMessageReplyServer::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myServerName);
	DESERIALIZE(aStream, myIP);
}