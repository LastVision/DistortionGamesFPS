
#include "stdafx.h"

#include "ActorComponent.h"
//#include "ActorComponentData.h"
#include <Animation.h>
#include "AnimationComponent.h"
#include <ArtifactMessage.h>
#include "AudioInterface.h"
#include "BehaviorNote.h"
#include "BlendedBehavior.h"
#include "../Game/FogOfWarMap.h"
#include "ControllerComponent.h"
#include <EmitterMessage.h>
#include <EngineEnums.h>
#include "GrenadeComponent.h"
#include "HealthComponent.h"
#include <Instance.h>
#include <InWorldTextMessage.h>
#include "KillUnitMessage.h"
#include "KilledPromotedMessage.h"
#include <ModelLoader.h>
#include <Terrain.h>
#include <TutorialMessage.h>
#include "PollingStation.h"
#include <PostMaster.h>
#include "PromotionComponent.h"
#include "SoundComponent.h"

ActorComponent::ActorComponent(Entity& aEntity, ActorComponentData& aData, const Prism::Terrain& aTerrain
	, const EntityCommandData& aCurrentCommand)
	: Component(aEntity)
	, myTerrain(aTerrain)
	, myVisionRange(aData.myVisionRange)
	, myVisionRange2(aData.myVisionRange * aData.myVisionRange)
	, myAttackRange2(aData.myAttackRange * aData.myAttackRange)
	, myAttackDamage(aData.myAttackDamage)
	, myAttackRechargeTime(aData.myAttackRechargeTime)
	, myAttackTimer(0.f)
	, myChaseDistance2(aData.myChaseDistance * aData.myChaseDistance)
	, myChaseDistanceNeutral2(aData.myChaseDistanceNeutral * aData.myChaseDistanceNeutral)
	, myAttackTargetPathRefreshTime(0.5f)
	, myCurrentAttackTargetPathRefreshTime(myAttackTargetPathRefreshTime)
	, myRangerOneShotTimer(0.f)
	, myRangerOneShotCooldown(aData.myRangerOneShotCoolDown)
	, myBehavior(new BlendedBehavior(myEntity))
	, myCurrentCommand(aCurrentCommand)
	, myIsReturning(false)
	, myMuzzleFlashes(8)
	, myCurrentMuzzleFlash(0)
	, myMuzzleTimer(0)
	, myMuzzleTimerShow(0)
	, myMuzzleFrameTimer(0)
	, myIsDone(false)
{
	myEntity.myMaxSpeed = aData.myMoveSpeed;

	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		myTargetType = eOwnerType::ENEMY | eOwnerType::NEUTRAL;
	}
	else if (myEntity.GetOwner() == eOwnerType::ENEMY)
	{
		myTargetType = eOwnerType::PLAYER | eOwnerType::NEUTRAL;
	}
	else if (myEntity.GetOwner() == eOwnerType::NEUTRAL)
	{
		myTargetType = eOwnerType::PLAYER | eOwnerType::ENEMY;
	}
	else
	{
		DL_ASSERT("An Entity not owned by Player, Enemy or Neutral tried to create a ControllerComponent");
	}

	myMuzzleRadius = 5.f;
	myMuzzleFlashes.Add(new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/SM_muzzleflash.fbx"
		, "Data/Resource/Shader/S_effect_pbl.fx"), myMuzzleOrientation, Prism::eOctreeType::DYNAMIC, myMuzzleRadius, false, false));
	myMuzzleFlashes.Add(new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/SM_muzzleflash2.fbx"
		, "Data/Resource/Shader/S_effect_pbl.fx"), myMuzzleOrientation, Prism::eOctreeType::DYNAMIC, myMuzzleRadius, false, false));
	myMuzzleFlashes.Add(new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/SM_muzzleflash3.fbx"
		, "Data/Resource/Shader/S_effect_pbl.fx"), myMuzzleOrientation, Prism::eOctreeType::DYNAMIC, myMuzzleRadius, false, false));
	myMuzzleFlashes.Add(new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/SM_muzzleflash4.fbx"
		, "Data/Resource/Shader/S_effect_pbl.fx"), myMuzzleOrientation, Prism::eOctreeType::DYNAMIC, myMuzzleRadius, false, false));
	myMuzzleFlashes.Add(new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/SM_muzzleflash5.fbx"
		, "Data/Resource/Shader/S_effect_pbl.fx"), myMuzzleOrientation, Prism::eOctreeType::DYNAMIC, myMuzzleRadius, false, false));

	for (int i = 0; i < myMuzzleFlashes.Size(); ++i)
	{
		myMuzzleFlashes[i]->SetShouldRender(false);
	}
}

ActorComponent::~ActorComponent()
{
	SAFE_DELETE(myBehavior);
	myMuzzleFlashes.DeleteAll();
}

void ActorComponent::Reset()
{
	myAcceleration = CU::Vector2<float>(0.f, 0.f);
	myIsDone = false;
	myBehavior->SetTarget({ myEntity.GetPosition().x + CU::Math::RandomRange<float>(-0.1f, 0.1f), myEntity.GetPosition().y - 1.f });

}

void ActorComponent::Update(float aDelta)
{
	if (myEntity.GetAlive() == false || myEntity.GetState() == eEntityState::DIE)
	{
		return;
	}

	MuzzleFlash(aDelta);

	CheckAnimationComplete();

	myRangerOneShotTimer -= aDelta;
	myAttackTimer -= aDelta;

	if (myCurrentCommand != myPreviousCommand)
	{
		myBehavior->SetTarget(myCurrentCommand.GetPosition());
		myPreviousCommand = myCurrentCommand;
	}

	myAcceleration = myBehavior->Update(aDelta);

	if (myBehavior->GetDone() || myIsDone == true)
	{
		myEntity.SendNote<BehaviorNote>(true);
		myIsDone = false;
	}

	myEntity.myVelocity += myAcceleration * aDelta;
	float velocity2 = CU::Length2(myEntity.myVelocity);
	if (velocity2 > myEntity.myMaxSpeed * myEntity.myMaxSpeed)
	{
		CU::Normalize(myEntity.myVelocity);
		myEntity.myVelocity *= myEntity.myMaxSpeed;
	}


	switch (myCurrentCommand.myCommand)
	{
	case eEntityCommand::ATTACK_TARGET:
		DoAttackTarget(aDelta);
		break;
	case eEntityCommand::STOP:
		DoStop(aDelta);
		break;
	case eEntityCommand::MOVE:
		DoMove(aDelta);
		break;
	case eEntityCommand::ATTACK_MOVE:
		DoAttackMove(aDelta);
		break;
	case eEntityCommand::HOLD_POSITION:
		DoHoldPosition(aDelta);
		break;
	default:
		DL_ASSERT("Unknown intentionstate!");
		break;
	}

	if (myEntity.GetOwner() != eOwnerType::NEUTRAL)
	{
		const CU::GrowingArray<Entity*>& artifacts = PollingStation::GetInstance()->GetArtifacts();

		for (int i = artifacts.Size() - 1; i >= 0; --i)
		{
			if (CU::Length2(myEntity.GetPosition() - artifacts[i]->GetPosition()) < 5 * 5)
			{
				if (myEntity.GetOwner() == eOwnerType::PLAYER)
				{
					PostMaster::GetInstance()->SendMessage(InWorldTextMessage("artifact pickup", myEntity.GetPosition()));
					PostMaster::GetInstance()->SendMessage(TutorialMessage(eTutorialAction::ARTIFACT));
					Prism::Audio::AudioInterface::GetInstance()->PostEvent("victoryGain", 0);
				}

				PostMaster::GetInstance()->SendMessage(ArtifactMessage(myEntity.GetOwner(), 1));
				//PostMaster::GetInstance()->SendMessage(EmitterMessage("artifact_glow", true, 1.f));
				artifacts[i]->RemoveFromScene();
				PostMaster::GetInstance()->SendMessage(EmitterMessage(artifacts[i]->GetEmitter(), true));

				PollingStation::GetInstance()->RemoveArtifact(artifacts[i]);
			}
		}
	}
}

void ActorComponent::CheckAnimationComplete()
{
	if (myEntity.GetState() == eEntityState::ATTACK
		|| myEntity.GetState() == eEntityState::THROW)
	{
		if (myEntity.GetComponent<AnimationComponent>()->IsCurrentAnimationDone() == true)
		{
			myEntity.SetState(eEntityState::IDLE);
		}
	}
}

Entity* ActorComponent::FindAttackTarget()
{
	Entity* closestTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
		, myTargetType, myVisionRange2);

	if (closestTarget != nullptr
		&& CU::Length2(closestTarget->GetPosition() - myEntity.GetPosition()) < myAttackRange2)
	{
		return closestTarget;
	}
	return nullptr;
}

void ActorComponent::DoStop(float aDelta)
{
	Entity* closestTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
		, myTargetType, myVisionRange2);

	bool attacking = false;

	if (closestTarget != nullptr)
	{
		if (CU::Length2(closestTarget->GetPosition() - myEntity.GetPosition()) < myAttackRange2)
		{
			attacking = true;
			if (myAttackTimer < 0)
			{
				AttackTarget(closestTarget, aDelta);
			}
			else
			{
				StandStill();
			}
		}
	}

	if (attacking == false)
	{

		if (CU::Length2(myEntity.GetPosition() - myCurrentCommand.GetPosition()) > myChaseDistance2)
		{
			myBehavior->SetTarget(myCurrentCommand.GetPosition());
			DoMove(aDelta);
			myIsReturning = true;
		}
		else if (myIsReturning == true)
		{
			myBehavior->SetTarget(myCurrentCommand.GetPosition());
			DoMove(aDelta);

			if (myBehavior->GetDone() == true)
			{
				myIsReturning = false;
			}
		}
		else
		{
			Entity* closestTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
				, myTargetType, myVisionRange2);

			if (closestTarget != nullptr)
			{
				if (CU::Length2(closestTarget->GetPosition() - myEntity.GetPosition()) < myAttackRange2)
				{
					if (myAttackTimer < 0)
					{
						AttackTarget(closestTarget, aDelta);
					}
					else
					{
						StandStill();
					}
				}
				else
				{
					myBehavior->SetTarget(closestTarget->GetPosition());
					DoMove(aDelta);
				}
			}
			else
			{
				/*int nearbyFriends = PollingStation::GetInstance()->FindClosestEntities(myEntity.GetOrientation().GetPos(),
					myEntity.GetOwner(), myVisionRange2).Size();*/
				if (myBehavior->GetDone() == false/* || CU::Length2(myEntity.GetPosition() - myCurrentCommand.GetPosition()) > 1.f * nearbyFriends*/)
				{
					myBehavior->SetTarget(myCurrentCommand.GetPosition());
					DoMove(aDelta);
				}
				else if (CU::Length(myAcceleration) > 1.f)
				{
					myBehavior->SetTarget(myEntity.GetPosition());
					DoMove(aDelta);
					myIsDone = true;
				}
				else
				{
					StandStill();
					myIsDone = true;
				}
			}
		}
	}
}

void ActorComponent::DoAttackTarget(float aDelta)
{
	if (myCurrentCommand.myEntity != nullptr
		&& myCurrentCommand.myEntity->GetAlive() == true
		&& CU::Length2(myCurrentCommand.myEntity->GetPosition() - myEntity.GetPosition()) < myAttackRange2)
	{
		if (myAttackTimer < 0)
		{
			AttackTarget(myCurrentCommand.myEntity, aDelta);
		}
		else
		{
			StandStill();
		}
	}
	else
	{
		DoMove(aDelta);
	}
}

void ActorComponent::DoAttackMove(float aDelta)
{
	Entity* closestTarget = FindAttackTarget();

	if (closestTarget != nullptr)
	{
		if (myAttackTimer < 0)
		{
			AttackTarget(closestTarget, aDelta);
		}
		else
		{
			StandStill();
		}
	}
	else
	{
		DoMove(aDelta);
	}
}


void ActorComponent::DoMove(float aDelta)
{
	LookInDirection(myEntity.myVelocity, aDelta);
	myEntity.SetState(eEntityState::WALK);
	CU::Vector2<float> position = myEntity.myPosition;

	position += myEntity.myVelocity * aDelta;
	myEntity.myOrientation.SetPos({ position.x, 0, position.y });
	myTerrain.CalcEntityHeight(myEntity.myOrientation);
}

void ActorComponent::DoHoldPosition(float aDelta)
{
	Entity* closestTarget = FindAttackTarget();

	if (closestTarget != nullptr)
	{
		if (myAttackTimer < 0)
		{
			AttackTarget(closestTarget, aDelta);
		}
		else
		{
			StandStill();
		}
	}
	else
	{
		StandStill();
	}
}

void ActorComponent::StandStill()
{
	//myAcceleration = { 0.f, 0.f };
	//myEntity.myVelocity = { 0.f, 0.f };


	if (myEntity.GetState() != eEntityState::ATTACK
		&& myEntity.GetState() != eEntityState::THROW)
	{
		myEntity.SetState(eEntityState::IDLE);
	}
}

void ActorComponent::AttackTarget(Entity* aTarget, float aDelta)
{
	if (aTarget->GetAlive() == false)
	{
		myEntity.SetState(eEntityState::IDLE);
		return;
	}
	if (myEntity.GetUnitType() == eUnitType::GRUNT && myEntity.GetState() == eEntityState::THROW &&
		myEntity.GetComponent<AnimationComponent>()->IsCurrentAnimationDone() == false)
	{
		return;
	}
	myEntity.SetState(eEntityState::ATTACK);
	LookInDirection(aTarget->GetPosition() - myEntity.GetPosition(), aDelta, true);

	myAttackTimer = myAttackRechargeTime;

	if (myEntity.GetUnitType() == eUnitType::GRUNT)
	{
		if (myEntity.GetComponent<PromotionComponent>()->GetPromoted() == true
			&& myEntity.GetComponent<GrenadeComponent>()->GetCooldown() <= 0.f)
		{
			myEntity.SetState(eEntityState::THROW);
			myEntity.GetComponent<GrenadeComponent>()->ThrowGrenade(aTarget->GetOrientation().GetPos());
			if (FogOfWarMap::GetInstance()->IsVisible(myEntity.GetPosition()) == true)
			{
				if (rand() % 2 == 0)
				{
					Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_ThrowGrenade"
						, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
				}
				PostMaster::GetInstance()->SendMessage(InWorldTextMessage("Grenade!", myEntity.GetPosition(), { 1.f, 0.88f, 0.f, 1.f }));
			}
		}
		else
		{
			if (FogOfWarMap::GetInstance()->IsVisible(myEntity.GetPosition()) == true)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_MachineGun"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
		}
	}
	else if (myEntity.GetUnitType() == eUnitType::RANGER)
	{
		if (myEntity.GetComponent<PromotionComponent>()->GetPromoted() == true && myRangerOneShotTimer <= 0.f)
		{
			if (FogOfWarMap::GetInstance()->IsVisible(myEntity.GetPosition()) == true)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_OneShot"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
		}
		else
		{
			if (FogOfWarMap::GetInstance()->IsVisible(myEntity.GetPosition()) == true)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_Sniper"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
		}

	}
	else if (myEntity.GetUnitType() == eUnitType::TANK)
	{
		if (FogOfWarMap::GetInstance()->IsVisible(myEntity.GetPosition()) == true)
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_MiniGun"
				, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
		}
	}

	HealthComponent* targetHealth = aTarget->GetComponent<HealthComponent>();
	bool targetSurvived = true; // if target is already dead, stay at "true" to not count as a kill
	if (targetHealth != nullptr && aTarget->GetAlive())
	{
		if (myEntity.GetUnitType() == eUnitType::RANGER
			&& myEntity.GetComponent<PromotionComponent>()->GetPromoted() == true
			&& myRangerOneShotTimer <= 0.f)
		{
			myRangerOneShotTimer = myRangerOneShotCooldown;
			targetSurvived = targetHealth->TakeDamageAndCheckSurvive((targetHealth->GetMaxHealth() * 0.5f) + targetHealth->GetArmor());

			if (FogOfWarMap::GetInstance()->IsVisible(myEntity.GetPosition()) == true )
			{
				if (rand() % 2 == 0)
				{
					Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_OneShotTalk"
						, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
				}
				PostMaster::GetInstance()->SendMessage(InWorldTextMessage("Critical hit!", aTarget->GetPosition(), { 1.f, 0.88f, 0.f, 1.f }));
			}

		}
		else
		{
			targetSurvived = targetHealth->TakeDamageAndCheckSurvive(myAttackDamage);
		}
	}

	AnimationComponent* animation = myEntity.GetComponent<AnimationComponent>();
	DL_ASSERT_EXP(animation != nullptr, "Animation missing from attacking unit");
	if (animation != nullptr)
	{
		animation->RestartCurrentAnimation();
	}

	if (targetSurvived == false)
	{
		myEntity.GetComponent<PromotionComponent>()->EnemyKilled();
		myBehavior->SetTarget(myEntity.GetPosition());
		PostMaster::GetInstance()->SendMessage(KillUnitMessage(static_cast<int>(aTarget->GetUnitType()),
			static_cast<int>(aTarget->GetOwner()), aTarget->GetComponent<ControllerComponent>()->GetTargetPosition()
			, static_cast<int>(myEntity.GetOwner())));

		if (aTarget->GetComponent<PromotionComponent>()->GetPromoted() == true)
		{
			PostMaster::GetInstance()->SendMessage(KilledPromotedMessage(myEntity.GetOwner(), 10, aTarget->GetPosition()));
		}
	}
}

void ActorComponent::LookInDirection(const CU::Vector2<float>& aDirection, float aDelta, bool aTurnInstant)
{
	CU::Vector2<float> direction2D = CU::GetNormalized(aDirection);
	CU::Vector3<float> direction = { direction2D.x, 0, direction2D.y };

	CU::Vector3<float> forward = myEntity.myOrientation.GetForward();
	CU::Normalize(forward);

	if (forward != direction)
	{
		CU::Vector3<float> planeNormal(0, 1.f, 0);
		float dot = CU::Dot(forward, direction);
		float det = CU::Dot(planeNormal, CU::Cross(forward, direction));

		float angle = atan2(det, dot);

		if (aTurnInstant == false)
		{
			float limit = static_cast<float>((M_PI)) * aDelta;
			if (angle > limit)
			{
				angle = limit;
			}
			else if (angle < -limit)
			{
				angle = -limit;
			}
		}

		CU::Vector3<float> pos = myEntity.myOrientation.GetPos();
		myEntity.myOrientation.SetPos({ 0.f, 0.f, 0.f });
		myEntity.myOrientation = myEntity.myOrientation * CU::Matrix44<float>::CreateRotateAroundY(angle);
		myEntity.myOrientation.SetPos(pos);
	}
}

void ActorComponent::Kill()
{
	for (int i = 0; i < myMuzzleFlashes.Size(); ++i)
	{
		myMuzzleFlashes[i]->SetShouldRender(false);
	}
}

void ActorComponent::MuzzleFlash(float aDelta)
{
	myMuzzleTimer -= aDelta;
	myMuzzleTimerShow -= aDelta;
	myMuzzleFrameTimer -= aDelta;

	for (int i = 0; i < myMuzzleFlashes.Size(); ++i)
	{
		myMuzzleFlashes[i]->SetShouldRender(false);
	}

	if (myEntity.GetState() == eEntityState::ATTACK && FogOfWarMap::GetInstance()->IsVisible(myEntity.GetPosition()))
	{
		if (myMuzzleFrameTimer < 0)
		{
			int prev = myCurrentMuzzleFlash;
			myCurrentMuzzleFlash = rand() % myMuzzleFlashes.Size();
			if (myCurrentMuzzleFlash == prev)
			{
				++myCurrentMuzzleFlash;
			}

			myMuzzleFrameTimer = 1.f / 10.f;
			if (myCurrentMuzzleFlash >= myMuzzleFlashes.Size())
			{
				myCurrentMuzzleFlash = 0;
			}
		}

		myMuzzleFlashes[myCurrentMuzzleFlash]->SetShouldRender(true);

		CU::Vector4<float> muzzlePosition;
		switch (myEntity.GetUnitType())
		{
		case eUnitType::GRUNT:
			muzzlePosition = CU::Vector4<float>(0.123f, 1.266f, 0.967f, 1.f);
			break;
		case eUnitType::RANGER:
			if (myMuzzleTimerShow < 0)
			{
				myMuzzleFlashes[myCurrentMuzzleFlash]->SetShouldRender(false);
			}
			muzzlePosition = CU::Vector4<float>(0.48f, 1.828f, 1.431f, 1.f);
			break;
		case eUnitType::TANK:
		case eUnitType::NON_ATTACK_TUTORIAL:
			muzzlePosition = CU::Vector4<float>(0.577f, 0.964f, 3.307f, 1.f);
			break;
		}


		if (myMuzzleTimer < 0)
		{
			myMuzzleTimer = 1.85f;
			myMuzzleTimerShow = 0.1f;
		}

		myMuzzleOrientation = myEntity.GetOrientation();

		myMuzzleOrientation.SetPos(muzzlePosition * myEntity.GetOrientation());
	}
}