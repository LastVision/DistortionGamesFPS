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

	void PhysicsInterface::Update()
	{
		if (myManager != nullptr)
		{
			myManager->Update();
		}
		else
		{
			DL_ASSERT("No PhysicsManager");
		}
	}

	Entity* PhysicsInterface::RayCast(const CU::Vector3<float>& aOrigin, const CU::Vector3<float>& aNormalizedDirection, float aMaxRayDistance)
	{
		if (myManager != nullptr)
		{
			return myManager->RayCast(aOrigin, aNormalizedDirection, aMaxRayDistance);
		}
		return nullptr;
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