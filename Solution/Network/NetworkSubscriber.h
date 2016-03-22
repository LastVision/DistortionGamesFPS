#pragma once

class NetMessageConnectReply;
class NetMessageDisconnect;
class NetMessageEntityState;
class NetMessageImportantReply;
class NetMessageLevelLoaded;
class NetMessageHealth;
class NetMessageHealthPack;
class NetMessageLevelComplete;
class NetMessageOnDeath;
class NetMessageOnHit;
class NetMessageOnJoin;
class NetMessagePingReply;
class NetMessagePingRequest;
class NetMessagePosition;
class NetMessageRequestConnect;
class NetMessageRequestLevel;
class NetMessageRequestStartGame;
class NetMessageSetActive;
class NetMessageStartGame;
class NetMessageEnemyShooting;

class NetworkSubscriber
{
public:
	NetworkSubscriber();
	~NetworkSubscriber();

	virtual void ReceiveNetworkMessage(const NetMessageConnectReply& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageDisconnect& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageEntityState& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageImportantReply& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageLevelLoaded& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageLevelComplete& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageHealth& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageHealthPack& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageOnDeath& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageOnHit& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageOnJoin& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessagePingReply& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessagePingRequest& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessagePosition& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageRequestConnect& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageRequestLevel& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageRequestStartGame& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageSetActive& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageStartGame& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageEnemyShooting& aMessage, const sockaddr_in& aSenderAddress);
};
