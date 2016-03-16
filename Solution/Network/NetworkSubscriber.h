#pragma once

class NetMessageImportantReply;
class NetMessageConnectMessage;
class NetMessageOnJoin;
class NetMessageDisconnect;
class NetMessageRequestLevel;
class NetMessagePingRequest;
class NetMessagePingReply;
class NetMessagePosition;
class NetMessageAddEnemy;
class NetMessageOnHit;
class NetMessageOnDeath;

class NetworkSubscriber
{
public:
	NetworkSubscriber();
	~NetworkSubscriber();

	virtual void ReceiveNetworkMessage(const NetMessageImportantReply& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageConnectMessage& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageOnJoin& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageDisconnect& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageRequestLevel& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessagePingRequest& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessagePingReply& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessagePosition& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageAddEnemy& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageOnHit& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageOnDeath& aMessage, const sockaddr_in& aSenderAddress);
};