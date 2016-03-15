#pragma once


struct FadeMessage;
struct EmitterMessage;
struct GameStateMessage;
struct OnClickMessage;
struct ResizeMessage;
struct Message;

struct PostMasterNetAddPlayerMessage;
struct PostMasterNetRemovePlayerMessage;
struct PostMasterNetAddEnemyMessage;
struct PostMasterNetSetPositionMessage;
struct PostMasterNetSendPositionMessage;
struct PostMasterNetOnHitMessage;
struct PostMasterNetOnDeathMessage;
struct PostMasterNetOnDisconnectMessage;

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
	virtual void ReceiveMessage(const PostMasterNetAddPlayerMessage& aMessage);
	virtual void ReceiveMessage(const PostMasterNetRemovePlayerMessage& aMessage);
	virtual void ReceiveMessage(const PostMasterNetAddEnemyMessage& aMessage);
	virtual void ReceiveMessage(const PostMasterNetSetPositionMessage& aMessage);
	virtual void ReceiveMessage(const PostMasterNetSendPositionMessage& aMessage);
	virtual void ReceiveMessage(const PostMasterNetOnHitMessage& aMessage);
	virtual void ReceiveMessage(const PostMasterNetOnDeathMessage& aMessage);
	virtual void ReceiveMessage(const PostMasterNetOnDisconnectMessage& aMessage);

};

