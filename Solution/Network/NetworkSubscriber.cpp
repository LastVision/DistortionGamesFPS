#include "stdafx.h"
#include "NetworkSubscriber.h"

NetworkSubscriber::NetworkSubscriber(){}
NetworkSubscriber::~NetworkSubscriber(){}

void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageConnectReply&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageDisconnect&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageDisplayMarker&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageDisplayRespawn&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageEntityState&, const sockaddr_in&) {}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageExplosion&, const sockaddr_in&) {}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageImportantReply&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageLevelLoaded&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageLevelComplete&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageHealth&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageHealthPack&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageOnDeath&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageOnHit&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageOnJoin&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageAllClientsComplete&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessagePingReply&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessagePingRequest&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessagePosition&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessagePressE&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageRayCastRequest&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageRequestConnect&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageSetLevel&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageStartLobby&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageRequestStartLevel&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageReplyServer&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageRequestServer&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageShootGrenade&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageSetActive&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageText&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageLoadLevel&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageEnemyShooting&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageActivateSpawnpoint&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessageActivateUnit&, const sockaddr_in&){}
void NetworkSubscriber::ReceiveNetworkMessage(const NetMessagePressEText&, const sockaddr_in&){}
