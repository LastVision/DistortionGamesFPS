#include "stdafx.h"
#include "NetworkSubscriber.h"

NetworkSubscriber::NetworkSubscriber(){}
NetworkSubscriber::~NetworkSubscriber(){}

void NetworkSubscriber::ReceiveNetworkMessage(const NetMessagePingReply&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageImportantReply&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageConnectMessage&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageDisconnect&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageRequestLevel&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessagePingRequest&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageOnJoin&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessagePosition&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageAddEnemy&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageOnHit&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageOnDeath&, const sockaddr_in&){}