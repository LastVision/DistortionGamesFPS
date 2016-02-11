#pragma once

namespace Prism
{
	class Terrain;

	namespace Navigation
	{
		class Triangle;
	}
}


struct ControllerComponentData;
class ControllerComponent : public Component
{
public:
	ControllerComponent(Entity& aEntity, ControllerComponentData& aData, const Prism::Terrain& aTerrain);
	~ControllerComponent();
	void Reset() override;

	void ReceiveNote(const BehaviorNote& aNote) override;

	void Update(float aDelta) override;

	void MoveTo(const CU::Vector3<float>& aPosition, bool aClearCommandQueue, bool& aHasPlayedSound);
	void AttackTarget(Entity* aEntity, bool aClearCommandQueue, bool& aHasPlayedSound);
	void AttackMove(const CU::Vector3<float>& aPosition, bool aClearCommandQueue, bool& aHasPlayedSound);
	void Stop(bool& aHasPlayedSound);
	void HoldPosition(bool& aHasPlayedSound);



	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
	const EntityCommandData& GetCurrentCommand() const;

	void SetRallyPoint(const CU::Vector2<float>& aRallyPoint);
	bool IsReady() const;

	CU::Vector2<float> GetTargetPosition() const;
	bool HasFoundPath() const;

private:
	void UpdateLastValidTriangle(float aDelta);
	void FillCommandList(eEntityCommand aAction, bool aClearCommandQueue, Entity* aEntity = nullptr
		, const CU::Vector2<float>& aTargetPosition = { -1.f, -1.f });

	void DoStop();
	void DoMove();
	void DoAttackTarget(float aDelta);
	void DoHold();
	void DoAttackMove();

	void AttackTarget();
	void StartNextAction();
	void RefreshPathToAttackTarget();
	void RenderDebugLines() const;
	eColorDebug GetActionColor(eEntityCommand aAction) const;
	
	CU::Vector2<float> myReturnPosition;
	CU::Vector2<float> myRallyPoint;

	const Prism::Terrain& myTerrain;
	eOwnerType myOwnerType;

	bool myStartNewAction;
	bool myFirstFrame;
	bool mySecondFrame;
	bool myIsReady;
	bool myFoundPath;

	Prism::Navigation::Triangle* myLastValidTriangle;
	float myTriangleTimer;
	float myCurrentTriangleTimer;

	CU::GrowingArray<EntityCommandData> myCommands;
	EntityCommandData myCurrentCommand;
};

inline eComponentType ControllerComponent::GetTypeStatic()
{
	return eComponentType::CONTROLLER;
}

inline eComponentType ControllerComponent::GetType()
{
	return GetTypeStatic();
}

inline const EntityCommandData& ControllerComponent::GetCurrentCommand() const
{
	return myCurrentCommand;
}

inline void ControllerComponent::SetRallyPoint(const CU::Vector2<float>& aRallyPoint)
{
	myRallyPoint = aRallyPoint;
}

inline bool ControllerComponent::IsReady() const
{
	return myIsReady;
}

inline CU::Vector2<float> ControllerComponent::GetTargetPosition() const
{
	if (myCommands.Size() > 0)
	{
		return myCommands[0].myPosition;
	}
	
	return myReturnPosition;
}

inline bool ControllerComponent::HasFoundPath() const
{
	return myFoundPath;
}