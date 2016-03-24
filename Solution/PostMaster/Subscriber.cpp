#include "stdafx.h"
#include "Subscriber.h"
#include "Message.h"

Subscriber::Subscriber(){}
Subscriber::~Subscriber(){}

void Subscriber::ReceiveMessage(const DefendTouchMessage&){};
void Subscriber::ReceiveMessage(const FadeMessage&){}
void Subscriber::ReceiveMessage(const EmitterMessage&){}
void Subscriber::ReceiveMessage(const EnemyKilledMessage&){}
void Subscriber::ReceiveMessage(const GameStateMessage&){}
void Subscriber::ReceiveMessage(const Message&){}
void Subscriber::ReceiveMessage(const OnClickMessage&){}
void Subscriber::ReceiveMessage(const OnRadioButtonMessage&){}
void Subscriber::ReceiveMessage(const ResizeMessage&){}
void Subscriber::ReceiveMessage(const StartGameMessage&){}
void Subscriber::ReceiveMessage(const SendTextToClientsMessage&){}
void Subscriber::ReceiveMessage(const SetActiveMessage&){}
void Subscriber::ReceiveMessage(const RespawnMessage&){}
void Subscriber::ReceiveMessage(const RespawnTriggerMessage&){}
