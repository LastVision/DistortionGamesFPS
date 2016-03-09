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

void Subscriber::ReceiveMessage(const NetworkAddPlayerMessage&){}
void Subscriber::ReceiveMessage(const NetworkRemovePlayerMessage&){}
void Subscriber::ReceiveMessage(const NetworkAddEnemyMessage&){}
void Subscriber::ReceiveMessage(const NetworkSetPositionMessage&){}
void Subscriber::ReceiveMessage(const NetworkSendPositionMessage&){}
void Subscriber::ReceiveMessage(const NetworkOnHitMessage&){}
