#include "stdafx.h"
#include "Subscriber.h"
#include "Message.h"

Subscriber::Subscriber(){}
Subscriber::~Subscriber(){}

void Subscriber::ReceiveMessage(const FadeMessage&){}
void Subscriber::ReceiveMessage(const EmitterMessage&){}
void Subscriber::ReceiveMessage(const GameStateMessage&){}
void Subscriber::ReceiveMessage(const Message&){}
void Subscriber::ReceiveMessage(const OnClickMessage&){}
void Subscriber::ReceiveMessage(const ResizeMessage&){}
void Subscriber::ReceiveMessage(const StartGameMessage&){}

void Subscriber::ReceiveMessage(const PostMasterNetAddPlayerMessage&){}
void Subscriber::ReceiveMessage(const PostMasterNetRemovePlayerMessage&){}
void Subscriber::ReceiveMessage(const PostMasterNetAddEnemyMessage&){}
void Subscriber::ReceiveMessage(const PostMasterNetSetPositionMessage&){}
void Subscriber::ReceiveMessage(const PostMasterNetSendPositionMessage&){}
void Subscriber::ReceiveMessage(const PostMasterNetOnHitMessage&){}
void Subscriber::ReceiveMessage(const PostMasterNetOnDeathMessage&){}
void Subscriber::ReceiveMessage(const PostMasterNetOnDisconnectMessage&) {}
void Subscriber::ReceiveMessage(const PostMasterNetStartGameMessage&) {}
void Subscriber::ReceiveMessage(const PostMasterNetRequestStartGameMessage&) {}