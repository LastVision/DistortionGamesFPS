#include "stdafx.h"

#include "PhysicsInterface.h"
#include "PhysicsManager.h"

namespace Prism
{
	PhysicsInterface* PhysicsInterface::myInstance = nullptr;
	void PhysicsInterface::Create(std::function<void(PhysicsComponent*, PhysicsComponent*, bool)> anOnTriggerCallback, bool aIsServer)
	{
		myInstance = new PhysicsInterface(anOnTriggerCallback, aIsServer);
	}

	void PhysicsInterface::Destroy()
	{
		SAFE_DELETE(myInstance);
	}

	PhysicsInterface* PhysicsInterface::GetInstance()
	{
		return myInstance;
	}

	PhysicsManager* PhysicsInterface::GetManager() const
	{
		return myManager;
	}

#ifdef THREAD_PHYSICS
	void PhysicsInterface::InitThread()
	{
		myManager->InitThread();
	}
	void PhysicsInterface::ShutdownThread()
	{
		myManager->ShutdownThread();
	}
#endif

	bool PhysicsInterface::GetInitDone() const
	{
		return myManager->GetInitDone();
	}

	void PhysicsInterface::EndFrame()
	{
#ifndef THREAD_PHYSICS
		myManager->Update();
#endif

#ifdef THREAD_PHYSICS
		myManager->SetLogicDone();
		//myManager->WaitForPhysics();
#endif

		//myManager->Swap();

#ifdef THREAD_PHYSICS
		myManager->SetSwapDone();
#endif

		myManager->EndFrame();
	}

	void PhysicsInterface::RayCast(const CU::Vector3<float>& aOrigin, const CU::Vector3<float>& aNormalizedDirection, float aMaxRayDistance
		, std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&, const CU::Vector3<float>&)> aFunctionToCall
		, const PhysicsComponent* aComponent)
	{
		if (myManager != nullptr)
		{
			myManager->RayCast(aOrigin, aNormalizedDirection, aMaxRayDistance, aFunctionToCall, aComponent);
		}
	}

	void PhysicsInterface::AddForce(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aDirection, float aMagnitude)
	{
		myManager->AddForce(aDynamicBody, aDirection, aMagnitude);
	}

	void PhysicsInterface::SetVelocity(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aVelocity)
	{
		myManager->SetVelocity(aDynamicBody, aVelocity);
	}

	void PhysicsInterface::TeleportToPosition(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aPosition)
	{
		myManager->TeleportToPosition(aDynamicBody, aPosition);
	}

	void PhysicsInterface::TeleportToPosition(physx::PxRigidStatic* aStaticBody, const CU::Vector3<float>& aPosition)
	{
		myManager->TeleportToPosition(aStaticBody, aPosition);
	}

	void PhysicsInterface::TeleportToPosition(int aID, const CU::Vector3<float>& aPosition)
	{
		myManager->TeleportToPosition(aID, aPosition);
	}

	void PhysicsInterface::MoveToPosition(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aPosition)
	{
		myManager->MoveToPosition(aDynamicBody, aPosition);
	}

	int PhysicsInterface::CreatePlayerController(const CU::Vector3<float>& aStartPosition, PhysicsComponent* aComponent, bool aShouldAddToScene)
	{
		return myManager->CreatePlayerController(aStartPosition, aComponent, aShouldAddToScene);
	}

	void PhysicsInterface::Move(int aId, const CU::Vector3<float>& aDirection, float aMinDisplacement, float aDeltaTime)
	{
		myManager->Move(aId, aDirection, aMinDisplacement, aDeltaTime);
	}

	void PhysicsInterface::MoveForward(bool move)
	{
		myManager->myMoveForward = move;
	}

	void PhysicsInterface::MoveBackward(bool move)
	{
		myManager->myMoveBackward = move;
	}

	void PhysicsInterface::UpdateOrientation(physx::PxRigidDynamic* aDynamicBody, physx::PxShape** aShape, float* aThread4x4)
	{
		myManager->UpdateOrientation(aDynamicBody, aShape, aThread4x4);
	}

	void PhysicsInterface::UpdateOrientation(physx::PxRigidStatic* aStaticBody, physx::PxShape** aShape, float* aThread4x4)
	{
		myManager->UpdateOrientation(aStaticBody, aShape, aThread4x4);
	}

	bool PhysicsInterface::GetAllowedToJump(int aId)
	{
		return myManager->GetAllowedToJump(aId);
	}

	void PhysicsInterface::SetPosition(int aId, const CU::Vector3<float>& aPosition)
	{
		myManager->SetPosition(aId, aPosition);
	}

	void PhysicsInterface::GetPosition(int aId, CU::Vector3<float>& aPositionOut)
	{
		myManager->GetPosition(aId, aPositionOut);
	}

	void PhysicsInterface::SubscribeToTriggers(physx::PxSimulationEventCallback* aSubscriber)
	{
		aSubscriber;
		//myManager->SubscribeToTriggers(aSubscriber);
	}

	void PhysicsInterface::Create(PhysicsComponent* aComponent, const PhysicsCallbackStruct& aPhysData
		, float* aOrientation, const std::string& aFBXPath
		, physx::PxRigidDynamic** aDynamicBodyOut, physx::PxRigidStatic** aStaticBodyOut
		, physx::PxShape*** someShapesOut, bool aShouldAddToScene, bool aShouldBeSphere)
	{
		myManager->Create(aComponent, aPhysData, aOrientation, aFBXPath, aDynamicBodyOut, aStaticBodyOut, someShapesOut, aShouldAddToScene, aShouldBeSphere);
	}

	void PhysicsInterface::Add(physx::PxRigidDynamic* aDynamic)
	{
		myManager->Add(aDynamic);
	}

	void PhysicsInterface::Add(physx::PxRigidStatic* aStatic)
	{
		myManager->Add(aStatic);
	}

	void PhysicsInterface::Add(int aCapsuleID)
	{
		myManager->Add(aCapsuleID);
	}

	void PhysicsInterface::Remove(physx::PxRigidDynamic* aDynamic, const PhysicsComponentData& aData)
	{
		myManager->Remove(aDynamic, aData);
	}
	
	void PhysicsInterface::Remove(physx::PxRigidStatic* aStatic, const PhysicsComponentData& aData)
	{
		myManager->Remove(aStatic, aData);
	}

	void PhysicsInterface::Remove(int aCapsuleID)
	{
		myManager->Remove(aCapsuleID);
	}

	void PhysicsInterface::Sleep(physx::PxRigidDynamic* aDynamic)
	{
		myManager->Sleep(aDynamic);
	}

	void PhysicsInterface::Sleep(int aCapsuleID)
	{
		myManager->Sleep(aCapsuleID);
	}

	void PhysicsInterface::Wake(physx::PxRigidDynamic* aDynamic)
	{
		myManager->Wake(aDynamic);
	}

	void PhysicsInterface::Wake(int aCapsuleID)
	{
		myManager->Wake(aCapsuleID);
	}

	int PhysicsInterface::GetFPS()
	{
		return myManager->physicsFPS;
	}

	PhysicsInterface::PhysicsInterface(std::function<void(PhysicsComponent*, PhysicsComponent*, bool)> anOnTriggerCallback, bool aIsServer)
	{
		myManager = new PhysicsManager(anOnTriggerCallback, aIsServer);
	}


	PhysicsInterface::~PhysicsInterface()
	{
		SAFE_DELETE(myManager);
	}
}