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

	PhysicsInterface::PhysicsInterface()
	{
		myManager = new PhysicsManager();
	}


	PhysicsInterface::~PhysicsInterface()
	{
		SAFE_DELETE(myManager);
	}
}