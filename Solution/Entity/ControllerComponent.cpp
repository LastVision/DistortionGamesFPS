#include "stdafx.h"

#include <AudioInterface.h>
#include "ActorComponent.h"
#include "AnimationComponent.h"
#include "BehaviorNote.h"
#include "ControllerComponent.h"
#include "GrenadeComponent.h"
#include "HealthComponent.h"
#include <InputWrapper.h>
#include <KillUnitMessage.h>
#include <PathFinderAStar.h>
#include <PathFinderFunnel.h>
#include "PollingStation.h"
#include "PromotionComponent.h"
#include <PostMaster.h>
#include "SoundComponent.h"
#include <Terrain.h>
#include <BlockMapMessage.h>



ControllerComponent::ControllerComponent(Entity& aEntity, ControllerComponentData&, const Prism::Terrain& aTerrain)
	: Component(aEntity)
	, myTerrain(aTerrain)
	, myCommands(16)
	, myLastValidTriangle(nullptr)
	, myCurrentTriangleTimer(0.f)
	, myTriangleTimer(0.25f)
	, myOwnerType(myEntity.GetOwner())
	, myStartNewAction(true)
	, myFirstFrame(true)
	, mySecondFrame(false)
	, myIsReady(false)
	, myFoundPath(false)
{
}


ControllerComponent::~ControllerComponent()
{
}

void ControllerComponent::Reset()
{
	myTerrain.CalcEntityHeight(myEntity.myOrientation);

	myStartNewAction = true;
	myFirstFrame = true;
	mySecondFrame = false;
	myIsReady = false;


	myCommands.RemoveAll();
	myCommands.Add(EntityCommandData(eEntityCommand::STOP, nullptr, myEntity.GetPosition()));
}

void ControllerComponent::ReceiveNote(const BehaviorNote& aNote)
{
	if (aNote.myFinished == true
		&& myCommands.Size() > 0
		&& myCurrentCommand.myCommand != eEntityCommand::HOLD_POSITION)
	{
		myStartNewAction = true;
	}
}

void ControllerComponent::Update(float aDelta)
{
	if (myEntity.GetAlive() == false || myEntity.GetState() == eEntityState::DIE)
	{
		return;
	}

	if (myEntity.GetOwner() != eOwnerType::NEUTRAL)
	{
		UpdateLastValidTriangle(aDelta);
	}

	if (mySecondFrame == true)
	{
		mySecondFrame = false;
		myIsReady = true;
		bool sound = false;
		MoveTo(CU::Vector3<float>(myRallyPoint.x, 0, myRallyPoint.y), true, sound);
	}

	if (myFirstFrame == true)
	{
		myFirstFrame = false;
		mySecondFrame = true;
		bool sound;
		Stop(sound);
	}

	if (myCurrentCommand.myCommand == eEntityCommand::ATTACK_TARGET
		&& myCurrentCommand.myEntity != nullptr
		&& myCurrentCommand.myEntity->GetAlive() == false)
	{
		myStartNewAction = true;
	}

	if (myStartNewAction == true)
	{
		myStartNewAction = false;
		StartNextAction();
	}

	if (myEntity.GetOwner() == eOwnerType::PLAYER && (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT) == true || CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_RSHIFT) == true))
	{
		RenderDebugLines();
	}
}

void ControllerComponent::MoveTo(const CU::Vector3<float>& aPosition, bool aClearCommandQueue, bool& aHasPlayedSound)
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		if (aHasPlayedSound == false)
		{
			if (myEntity.GetUnitType() == eUnitType::GRUNT)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_Move", 0);
			}
			else if (myEntity.GetUnitType() == eUnitType::RANGER)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_Move", 0);
			}
			else if (myEntity.GetUnitType() == eUnitType::TANK)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_Move", 0);
			}
			aHasPlayedSound = true;
		}
	}
	FillCommandList(eEntityCommand::MOVE, aClearCommandQueue, nullptr, { aPosition.x, aPosition.z });
}

void ControllerComponent::AttackTarget(Entity* aEntity, bool aClearCommandQueue, bool& aHasPlayedSound)
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		if (aHasPlayedSound == false)
		{
			if (myEntity.GetUnitType() == eUnitType::GRUNT)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_Attack", 0);
			}
			else if (myEntity.GetUnitType() == eUnitType::RANGER)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_Attack", 0);
			}
			else if (myEntity.GetUnitType() == eUnitType::TANK)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_Attack", 0);
			}
			aHasPlayedSound = true;
		}
	}
	FillCommandList(eEntityCommand::ATTACK_TARGET, aClearCommandQueue, aEntity);
}

void ControllerComponent::AttackMove(const CU::Vector3<float>& aPosition, bool aClearCommandQueue, bool& aHasPlayedSound)
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		if (aHasPlayedSound == false)
		{
			if (myEntity.GetUnitType() == eUnitType::GRUNT)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_AttackMove", 0);
			}
			else if (myEntity.GetUnitType() == eUnitType::RANGER)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_AttackMove", 0);
			}
			else if (myEntity.GetUnitType() == eUnitType::TANK)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_AttackMove", 0);
			}
			aHasPlayedSound = true;
		}
	}
	FillCommandList(eEntityCommand::ATTACK_MOVE, aClearCommandQueue, nullptr, { aPosition.x, aPosition.z });
}

void ControllerComponent::Stop(bool& aHasPlayedSound)
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		if (aHasPlayedSound == false)
		{
			if (myEntity.GetUnitType() == eUnitType::GRUNT)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_Stop", 0);
			}
			else if (myEntity.GetUnitType() == eUnitType::RANGER)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_Stop", 0);
			}
			else if (myEntity.GetUnitType() == eUnitType::TANK)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_Stop", 0);
			}
			aHasPlayedSound = true;
		}
	}

	myCommands.RemoveAll();
	myStartNewAction = true;
	myCommands.Add(EntityCommandData(eEntityCommand::STOP, nullptr, myEntity.myPosition));
}

void ControllerComponent::HoldPosition(bool& aHasPlayedSound)
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		if (aHasPlayedSound == false)
		{
			if (myEntity.GetUnitType() == eUnitType::GRUNT)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_Hold", 0);
			}
			else if (myEntity.GetUnitType() == eUnitType::RANGER)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_Hold", 0);
			}
			else if (myEntity.GetUnitType() == eUnitType::TANK)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_Hold", 0);
			}
			aHasPlayedSound = true;
		}
	}

	myCommands.RemoveAll();
	myStartNewAction = true;
	myCommands.Add(EntityCommandData(eEntityCommand::HOLD_POSITION, nullptr, myEntity.myPosition));
}

void ControllerComponent::UpdateLastValidTriangle(float aDelta)
{
	myCurrentTriangleTimer -= aDelta;
	if (myCurrentTriangleTimer <= 0.f)
	{
		myCurrentTriangleTimer = myTriangleTimer;
		Prism::Navigation::Triangle* tri = myTerrain.GetPathFinder()->GetCurrentTriangle(myEntity.GetPosition());
		if (tri != nullptr)
		{
			myLastValidTriangle = tri;
		}
	}
}

void ControllerComponent::FillCommandList(eEntityCommand aAction, bool aClearCommandQueue, Entity* aEntity
	, const CU::Vector2<float>& aTargetPosition)
{
	myFoundPath = true;
	if (myEntity.GetOwner() == eOwnerType::ENEMY)
	{
		PostMaster::GetInstance()->SendMessage(BlockMapMessage(GetTargetPosition(), false));
	}

	CU::Vector2<float> checkTargetPosition(aTargetPosition);
	if (aEntity != nullptr)
	{
		checkTargetPosition = aEntity->GetPosition();
	}
	if (myTerrain.GetPathFinder()->IsOutside(checkTargetPosition) == true)
	{
		myFoundPath = false;
		return;
	}

	if (aClearCommandQueue == true)
	{
		myCommands.RemoveAll();
		myStartNewAction = true;
	}

	if (myCommands.Size() < 1)
	{
		CU::Vector2<float> position(aTargetPosition);
		if (aEntity != nullptr)
		{
			position = aEntity->GetPosition();
		}
		myCommands.Add(EntityCommandData(eEntityCommand::STOP, nullptr, position));
	}

	EntityCommandData action;
	action.myCommand = aAction;
	action.myEntity = nullptr;

	CU::Vector3<float> targetPosition(aTargetPosition.x, 0, aTargetPosition.y);
	CU::Vector3<float> startPosition(myEntity.GetOrientation().GetPos());

	if (aEntity != nullptr)
	{
		targetPosition = aEntity->GetOrientation().GetPos();
	}

	if (myTerrain.GetPathFinder()->IsOutside(myEntity.GetPosition()) == true)
	{
		DL_ASSERT_EXP(myLastValidTriangle != nullptr, "Entity Outside Navmesh without LastValidTriangle");
		if (myLastValidTriangle != nullptr)
		{
			startPosition = myLastValidTriangle->GetCenter();
		}
	}

	if (myCommands.Size() > 1)
	{
		startPosition.x = myCommands[0].GetPosition().x;
		startPosition.z = myCommands[0].GetPosition().y;
	}

	CU::GrowingArray<CU::Vector3<float>> path(16);
	if (myTerrain.GetPathFinder()->FindPath(startPosition, targetPosition, path) == true)
	{
		if (path.Size() > 0)
		{
			for (int i = 0; i < path.Size(); ++i)
			{
				CU::Vector2<float> target = { path[i].x, path[i].z };

				action.myPosition = target;

				if (myCommands.Size() > 1)
				{
					if (i == path.Size() - 1)
					{
						action.myEntity = aEntity;
					}
					myCommands.Insert(1, action);
				}
				else
				{
					if (i == path.Size() - 1)
					{
						action.myEntity = aEntity;
					}
					myCommands.Add(action);
				}
			}
		}
		else
		{
			action.myEntity = aEntity;
			action.myPosition = aTargetPosition;

			if (myCommands.Size() > 1)
			{
				myCommands.Insert(1, action);
			}
			else
			{
				myCommands.Add(action);
			}
		}
	}
	/*else
	{
		myCommands.RemoveAll();
		
		else
		{
			myCommands.Add(EntityCommandData(eEntityCommand::STOP, nullptr, myEntity.GetPosition()));
			myFoundPath = false;
		}
	}*/

	if (aClearCommandQueue == false)
	{
		if (myCommands.Size() > 0)
		{
			CU::Vector2<float> position(aTargetPosition);
			if (aEntity != nullptr)
			{
				position = aEntity->GetPosition();
			}
			myCommands[0] = EntityCommandData(eEntityCommand::STOP, nullptr, position);
		}
	}

	if (myEntity.GetOwner() == eOwnerType::ENEMY
		&& (aAction == eEntityCommand::ATTACK_MOVE || aAction == eEntityCommand::MOVE))
	{
		PostMaster::GetInstance()->SendMessage(BlockMapMessage(GetTargetPosition(), true));
	}
}

void ControllerComponent::StartNextAction()
{
	if (myCommands.Size() > 0)
	{
		myReturnPosition = myCommands.GetLast().GetPosition();
		myCurrentCommand = myCommands.GetLast();
		myCommands.RemoveCyclicAtIndex(myCommands.Size() - 1);
	}
	else
	{
		myCurrentCommand = EntityCommandData(eEntityCommand::STOP, nullptr, myReturnPosition);
	}
}

// TODO: Var ska detta ligga?
//void ControllerComponent::RefreshPathToAttackTarget()
//{
//	bool shouldBeQuiet = true;
//	AttackTarget(myCurrentCommand.myEntity, true, shouldBeQuiet);
//	myCurrentAttackTargetPathRefreshTime = myAttackTargetPathRefreshTime;
//}

void ControllerComponent::RenderDebugLines() const
{
	CU::Vector3<float> pointA;
	CU::Vector3<float> pointB;

	if (myCurrentCommand.myCommand != eEntityCommand::STOP)
	{
		pointA = myTerrain.GetHeight(myEntity.myOrientation.GetPos(), 1.f);
		pointB = myTerrain.GetHeight(myCurrentCommand.GetPosition(), 1.f);

		Prism::RenderLine3D(pointA, pointB, GetActionColor(myCurrentCommand.myCommand));
	}

	if (myCommands.Size() > 0)
	{
		pointA = myTerrain.GetHeight(myEntity.myOrientation.GetPos(), 1.f);
		if ((myCurrentCommand.myPosition != CU::Vector2<float>(-1, -1) || myCurrentCommand.myEntity != nullptr) == true)
		{
			pointA = myTerrain.GetHeight(myCurrentCommand.GetPosition(), 1.f);
		}

		pointB = myTerrain.GetHeight(myCommands.GetLast().GetPosition(), 1.f);
		eColorDebug color = GetActionColor(myCommands.GetLast().myCommand);

		Prism::RenderLine3D(pointA, pointB, color);

		for (int i = myCommands.Size() - 1; i >= 1; --i)
		{
			color = GetActionColor(myCommands[i - 1].myCommand);


			pointA = myTerrain.GetHeight(myCommands[i].GetPosition(), 1.f);
			pointB = myTerrain.GetHeight(myCommands[i - 1].GetPosition(), 1.f);
			Prism::RenderLine3D(pointA, pointB, color);
		}
	}
}

eColorDebug ControllerComponent::GetActionColor(eEntityCommand aAction) const
{
	switch (aAction)
	{
	case eEntityCommand::STOP:
		return eColorDebug::WHITE;
		break;
	case eEntityCommand::MOVE:
		return eColorDebug::GREEN;
		break;
	case eEntityCommand::ATTACK_TARGET:
		return eColorDebug::RED;
		break;
	case eEntityCommand::ATTACK_MOVE:
		return eColorDebug::RED;
		break;
	case eEntityCommand::HOLD_POSITION:
		return eColorDebug::YELLOW;
		break;
	default:
		DL_ASSERT("Tried to GetActionColor of a invalid action");
		break;
	}

	return eColorDebug::NOT_USED;
}