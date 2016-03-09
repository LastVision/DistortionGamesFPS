#pragma once


struct FadeMessage;
struct EmitterMessage;
struct GameStateMessage;
struct OnClickMessage;
struct ResizeMessage;
struct Message;

struct NetworkAddPlayerMessage;
struct NetworkRemovePlayerMessage;
struct NetworkAddEnemyMessage;
struct NetworkSetPositionMessage;
struct NetworkSendPositionMessage;
struct NetworkOnHitMessage;

class Subscriber
{
public:
	Subscriber();
	~Subscriber();

	virtual void ReceiveMessage(const FadeMessage& aMessage);
	virtual void ReceiveMessage(const EmitterMessage& aMessage);
	virtual void ReceiveMessage(const GameStateMessage& aMessage);
	virtual void ReceiveMessage(const Message& aMessage);
	virtual void ReceiveMessage(const OnClickMessage& aMessage);
	virtual void ReceiveMessage(const ResizeMessage& aMessage);

	/* Network Based Messages below*/
	virtual void ReceiveMessage(const NetworkAddPlayerMessage& aMessage);
	virtual void ReceiveMessage(const NetworkRemovePlayerMessage& aMessage);
	virtual void ReceiveMessage(const NetworkAddEnemyMessage& aMessage);
	virtual void ReceiveMessage(const NetworkSetPositionMessage& aMessage);
	virtual void ReceiveMessage(const NetworkSendPositionMessage& aMessage);
	virtual void ReceiveMessage(const NetworkOnHitMessage& aMessage);

};

