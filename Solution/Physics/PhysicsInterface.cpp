#include "stdafx.h"

#include "PhysicsInterface.h"
#include "PhysicsManager.h"

namespace Prism
{
	PhysicsInterface* PhysicsInterface::myInstance = nullptr;
	void PhysicsInterface::Create(std::function<void(PhysicsComponent*, PhysicsComponent*)> anOnTriggerCallback)
	{
		myInstance = new PhysicsInterface(anOnTriggerCallback);
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

	void PhysicsInterface::EndFrame()
	{
#ifndef THREAD_PHYSICS
		myManager->Update();
#endif

#ifdef THREAD_PHYSICS
		myManager->SetLogicDone();
		myManager->WaitForPhysics();
#endif

		myManager->Swap();

#ifdef THREAD_PHYSICS
		myManager->SetSwapDone();
#endif

		myManager->EndFrame();
	}

	void PhysicsInterface::RayCast(const CU::Vector3<float>& aOrigin, const CU::Vector3<float>& aNormalizedDirection, float aMaxRayDistance, std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&)> aFunctionToCall)
	{
		if (myManager != nullptr)
		{
			myManager->RayCast(aOrigin, aNormalizedDirection, aMaxRayDistance, aFunctionToCall);
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

	void PhysicsInterface::SetPosition(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aPosition)
	{
		myManager->SetPosition(aDynamicBody, aPosition);
	}

	int PhysicsInterface::CreatePlayerController(const CU::Vector3<float>& aStartPosition, PhysicsComponent* aComponent)
	{
		return myManager->CreatePlayerController(aStartPosition, aComponent);
	}

	void PhysicsInterface::Move(int aId, const CU::Vector3<float>& aDirection, float aMinDisplacement, float aDeltaTime)
	{
		myManager->Move(aId, aDirection, aMinDisplacement, aDeltaTime);
	}

	void PhysicsInterface::UpdateOrientation(physx::PxRigidDynamic* aDynamicBody, physx::PxShape** aShape, float* aThread4x4)
	{
		myManager->UpdateOrientation(aDynamicBody, aShape, aThread4x4);
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
		//myManager->SubscribeToTriggers(aSubscriber);
	}

	void PhysicsInterface::Create(PhysicsComponent* aComponent, const PhysicsCallbackStruct& aPhysData
		, float* aOrientation, const std::string& aFBXPath
		, physx::PxRigidDynamic** aDynamicBodyOut, physx::PxRigidStatic** aStaticBodyOut
		, physx::PxShape*** someShapesOut)
	{
		myManager->Create(aComponent, aPhysData, aOrientation, aFBXPath, aDynamicBodyOut, aStaticBodyOut, someShapesOut);
	}

	void PhysicsInterface::Remove(physx::PxRigidDynamic* aDynamic, const PhysicsComponentData& aData)
	{
		myManager->Remove(aDynamic, aData);
	}
	
	void PhysicsInterface::Remove(physx::PxRigidStatic* aStatic, const PhysicsComponentData& aData)
	{
		myManager->Remove(aStatic, aData);
	}

	PhysicsInterface::PhysicsInterface(std::function<void(PhysicsComponent*, PhysicsComponent*)> anOnTriggerCallback)
	{
		myManager = new PhysicsManager(anOnTriggerCallback);
	}


	PhysicsInterface::~PhysicsInterface()
	{
		SAFE_DELETE(myManager);
	}
}