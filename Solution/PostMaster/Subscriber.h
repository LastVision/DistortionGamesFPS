#pragma once

struct DefendTouchMessage;
struct FadeMessage;
struct EmitterMessage;
struct EnemyKilledMessage;
struct GameStateMessage;
struct OnClickMessage;
struct OnRadioButtonMessage;
struct ResizeMessage;
struct StartGameMessage;
struct SendTextToClientsMessage;
struct SetActiveMessage;
struct PrintTextMessage;
struct RespawnMessage;
struct RespawnTriggerMessage;
struct ActivateSpawnpointMessage;
struct Message;
struct LevelLoadedMessage;
struct LevelCompleteMessage;
struct HitmarkerMessage;

class Subscriber
{
public:
	Subscriber();
	~Subscriber();

	virtual void ReceiveMessage(const DefendTouchMessage& aMessage);
	virtual void ReceiveMessage(const FadeMessage& aMessage);
	virtual void ReceiveMessage(const EmitterMessage& aMessage);
	virtual void ReceiveMessage(const EnemyKilledMessage& aMessage);
	virtual void ReceiveMessage(const GameStateMessage& aMessage);
	virtual void ReceiveMessage(const Message& aMessage);
	virtual void ReceiveMessage(const OnClickMessage& aMessage);
	virtual void ReceiveMessage(const OnRadioButtonMessage& aMessage);
	virtual void ReceiveMessage(const ResizeMessage& aMessage);
	virtual void ReceiveMessage(const StartGameMessage& aMessage);
	virtual void ReceiveMessage(const SendTextToClientsMessage& aMessage);
	virtual void ReceiveMessage(const SetActiveMessage& aMessage);
	virtual void ReceiveMessage(const PrintTextMessage& aMessage);
	virtual void ReceiveMessage(const RespawnMessage& aMessage);
	virtual void ReceiveMessage(const RespawnTriggerMessage& aMessage);
	virtual void ReceiveMessage(const ActivateSpawnpointMessage& aMessage);
	virtual void ReceiveMessage(const LevelLoadedMessage& aMessage);
	virtual void ReceiveMessage(const LevelCompleteMessage& aMessage);
	virtual void ReceiveMessage(const HitmarkerMessage& aMessage);
};

