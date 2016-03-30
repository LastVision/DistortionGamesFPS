#pragma once
#include "NetworkMessageTypes.h"
#include "NetImportantMessage.h"

class NetMessageReplyServer : public NetImportantMessage
{
public:
	NetMessageReplyServer();
	NetMessageReplyServer(const std::string& aServerName);

	std::string myServerName;
protected:

	void DoSerialize(StreamType& aStream) override;
	void DoDeSerialize(StreamType& aStream) override;
};

inline NetMessageReplyServer::NetMessageReplyServer()
	: NetImportantMessage(eNetMessageType::SERVER_REPLY)
{
}

inline NetMessageReplyServer::NetMessageReplyServer(const std::string& aServerName)
	: NetImportantMessage(eNetMessageType::SERVER_REPLY)
	, myServerName(aServerName)
{
}

inline void NetMessageReplyServer::DoSerialize(StreamType& aStream)
{
	__super::DoSerialize(aStream);
	SERIALIZE(aStream, myServerName);
}

inline void NetMessageReplyServer::DoDeSerialize(StreamType& aStream)
{
	__super::DoDeSerialize(aStream);
	DESERIALIZE(aStream, myServerName);
}