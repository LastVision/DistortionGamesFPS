#include "stdafx.h"

#include "PhysicsInterface.h"
#include "PhysicsManager.h"

namespace Prism
{
	PhysicsInterface* PhysicsInterface::myInstance = nullptr;
	void PhysicsInterface::Create()
	{
		myInstance = new PhysicsInterface();
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
#endif

	void PhysicsInterface::EndFrame()
	{
#ifndef THREAD_PHYSICS
		myManager->Update();
#endif
		myManager->SwapOrientations();
	}

	void PhysicsInterface::RayCast(const CU::Vector3<float>& aOrigin, const CU::Vector3<float>& aNormalizedDirection, float aMaxRayDistance, std::function<void(Entity*, const CU::Vector3<float>&)> aFunctionToCall)
	{
		if (myManager != nullptr)
		{
			myManager->RayCast(aOrigin, aNormalizedDirection, aMaxRayDistance, aFunctionToCall);
		}
	}

	int PhysicsInterface::CreatePlayerController(const CU::Vector3<float>& aStartPosition)
	{
		return myManager->CreatePlayerController(aStartPosition);
	}

	void PhysicsInterface::Move(int aId, const CU::Vector3<float>& aDirection, float aMinDisplacement, float aDeltaTime)
	{
		//myManager->Move(aId, aDirection, aMinDisplacement, aDeltaTime);
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
		//myManager->GetPosition(aId, aPositionOut);
	}

	PhysicsInterface::PhysicsInterface()
	{
		myManager = new PhysicsManager();
	}


	PhysicsInterface::~PhysicsInterface()
	{
		SAFE_DELETE(myManager);
	}
}