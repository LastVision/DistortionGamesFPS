#pragma once

class NetMessageConnectReply;
class NetMessageDisconnect;
class NetMessageEntityState;
class NetMessageExplosion;
class NetMessageImportantReply;
class NetMessageLevelLoaded;
class NetMessageHealth;
class NetMessageHealthPack;
class NetMessageLevelComplete;
class NetMessageOnDeath;
class NetMessageOnHit;
class NetMessageOnJoin;
class NetMessageAllClientsComplete;
class NetMessagePingReply;
class NetMessagePingRequest;
class NetMessagePosition;
class NetMessageRequestConnect;
class NetMessageSetLevel;
class NetMessageRequestStartLevel;
class NetMessageShootGrenade;
class NetMessageSetActive;
class NetMessageText;
class NetMessageLoadLevel;
class NetMessageEnemyShooting;

class NetworkSubscriber
{
public:
	NetworkSubscriber();
	~NetworkSubscriber();

	virtual void ReceiveNetworkMessage(const NetMessageConnectReply& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageDisconnect& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageEntityState& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageExplosion& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageImportantReply& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageLevelLoaded& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageLevelComplete& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageHealth& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageHealthPack& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageOnDeath& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageOnHit& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageOnJoin& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageAllClientsComplete& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessagePingReply& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessagePingRequest& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessagePosition& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageRequestConnect& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageSetLevel& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageRequestStartLevel& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageShootGrenade& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageSetActive& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageText& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageLoadLevel& aMessage, const sockaddr_in& aSenderAddress);
	virtual void ReceiveNetworkMessage(const NetMessageEnemyShooting& aMessage, const sockaddr_in& aSenderAddress);
};
