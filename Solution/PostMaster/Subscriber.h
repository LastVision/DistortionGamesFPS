#pragma once

struct ArtifactMessage;
struct BlockMapMessage;
struct CinematicMessage;
struct FadeMessage;
struct EmitterMessage;
struct EventPositionMessage;
struct GameStateMessage;
struct InWorldTextMessage;
struct KillUnitMessage;
struct KilledPromotedMessage;
struct Message;
struct MinimapEventMessage;
struct MinimapMoveMessage;
struct MoveCameraMessage;
struct NotificationMessage;
struct OnClickMessage;
struct ResizeMessage;
struct ResourceMessage;
struct RunScriptMessage;
struct SelectUnitMessage;
struct SpawnUnitMessage;
struct TextMessage;
struct TimeMultiplierMessage;
struct ToggleGUIMessage;
struct ToggleBuildTimeMessage;
struct ToggleFogOfWarMessage;
struct ToggleRenderLinesMessage;
struct TriggerMessage;
struct TutorialMessage;
struct VictoryMessage;
struct UpgradeUnitMessage;

class Subscriber
{
public:
	Subscriber();
	~Subscriber();

	virtual void ReceiveMessage(const ArtifactMessage& aMessage);
	virtual void ReceiveMessage(const BlockMapMessage& aMessage);
	virtual void ReceiveMessage(const CinematicMessage& aMessage);
	virtual void ReceiveMessage(const FadeMessage& aMessage);
	virtual void ReceiveMessage(const EmitterMessage& aMessage);
	virtual void ReceiveMessage(const EventPositionMessage& aMessage);
	virtual void ReceiveMessage(const GameStateMessage& aMessage);
	virtual void ReceiveMessage(const InWorldTextMessage& aMessage);
	virtual void ReceiveMessage(const KillUnitMessage& aMessage);
	virtual void ReceiveMessage(const KilledPromotedMessage& aMessage);
	virtual void ReceiveMessage(const Message& aMessage);
	virtual void ReceiveMessage(const MinimapEventMessage& aMessage);
	virtual void ReceiveMessage(const MinimapMoveMessage& aMessage);
	virtual void ReceiveMessage(const MoveCameraMessage& aMessage);
	virtual void ReceiveMessage(const NotificationMessage& aMessage);
	virtual void ReceiveMessage(const OnClickMessage& aMessage);
	virtual void ReceiveMessage(const ResizeMessage& aMessage);
	virtual void ReceiveMessage(const ResourceMessage& aMessage);
	virtual void ReceiveMessage(const RunScriptMessage& aMessage);
	virtual void ReceiveMessage(const SelectUnitMessage& aMessage);
	virtual void ReceiveMessage(const SpawnUnitMessage& aMessage);
	virtual void ReceiveMessage(const TextMessage& aMessage);
	virtual void ReceiveMessage(const TimeMultiplierMessage& aMessage);
	virtual void ReceiveMessage(const ToggleBuildTimeMessage& aMessage);
	virtual void ReceiveMessage(const ToggleGUIMessage& aMessage);
	virtual void ReceiveMessage(const ToggleFogOfWarMessage& aMessage);
	virtual void ReceiveMessage(const ToggleRenderLinesMessage& aMessage);
	virtual void ReceiveMessage(const TriggerMessage& aMessage);
	virtual void ReceiveMessage(const TutorialMessage& aMessage);
	virtual void ReceiveMessage(const VictoryMessage& aMessage);
	virtual void ReceiveMessage(const UpgradeUnitMessage& aMessage);
};

