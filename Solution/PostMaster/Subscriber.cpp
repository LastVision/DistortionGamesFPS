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
void Subscriber::ReceiveMessage(const NetworkAddEnemyMessage&){}
void Subscriber::ReceiveMessage(const NetworkSetPositionMessage& aMessage){}
void Subscriber::ReceiveMessage(const NetworkSendPositionMessage& aMessage){}
