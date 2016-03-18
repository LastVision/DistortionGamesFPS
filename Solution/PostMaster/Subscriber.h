#pragma once


struct FadeMessage;
struct EmitterMessage;
struct EnemyKilledMessage;
struct GameStateMessage;
struct OnClickMessage;
struct OnRadioButtonMessage;
struct ResizeMessage;
struct StartGameMessage;
struct SetActiveMessage;
struct Message;

class Subscriber
{
public:
	Subscriber();
	~Subscriber();

	virtual void ReceiveMessage(const FadeMessage& aMessage);
	virtual void ReceiveMessage(const EmitterMessage& aMessage);
	virtual void ReceiveMessage(const EnemyKilledMessage& aMessage);
	virtual void ReceiveMessage(const GameStateMessage& aMessage);
	virtual void ReceiveMessage(const Message& aMessage);
	virtual void ReceiveMessage(const OnClickMessage& aMessage);
	virtual void ReceiveMessage(const OnRadioButtonMessage& aMessage);
	virtual void ReceiveMessage(const ResizeMessage& aMessage);
	virtual void ReceiveMessage(const StartGameMessage& aMessage);
	virtual void ReceiveMessage(const SetActiveMessage& aMessage);
};

