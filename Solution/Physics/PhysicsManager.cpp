#include "stdafx.h"
#include "PhysicsManager.h"

#ifdef _DEBUG
#pragma comment(lib, "PhysX\\vc12win32\\PhysX3DEBUG_x86.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PhysX3CookingDEBUG_x86.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PhysXProfileSDKDEBUG.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PxTaskDEBUG.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PhysXVisualDebuggerSDKDEBUG.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PhysX3CharacterKinematicDEBUG_x86.lib")
#else
#pragma comment(lib, "PhysX\\vc12win32\\PhysX3_x86.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PhysX3Common_x86.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PhysX3Cooking_x86.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PhysXProfileSDK.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PxTask.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PhysX3Extensions.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PhysXVisualDebuggerSDK.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PhysX3CharacterKinematic_x86.lib")
#endif

#include "PhysEntity.h"
#include <pxphysicsapi.h>
#include <PxQueryReport.h>

namespace Prism
{
	PhysicsManager::PhysicsManager()
	{
		myTimestep = 1.f / 60.f;
		
		myFoundation = PxCreateFoundation(0x03030300, myDefaultAllocatorCallback, myDefaultErrorCallback);
		myProfileZoneManager = &physx::PxProfileZoneManager::createProfileZoneManager(myFoundation);
		bool recordMemoryAllocations = true;
		myPhysicsSDK = PxCreatePhysics(0x03030300, *myFoundation, physx::PxTolerancesScale(),
			recordMemoryAllocations, myProfileZoneManager);

		PxInitExtensions(*myPhysicsSDK);

		physx::PxCookingParams params(myPhysicsSDK->getTolerancesScale());
		params.meshWeldTolerance = 0.001f;
		params.meshPreprocessParams = physx::PxMeshPreprocessingFlags(
			physx::PxMeshPreprocessingFlag::eWELD_VERTICES | physx::PxMeshPreprocessingFlag::eREMOVE_UNREFERENCED_VERTICES | physx::PxMeshPreprocessingFlag::eREMOVE_DUPLICATED_TRIANGLES);

		myCooker = nullptr;
		myCooker = PxCreateCooking(0x03030300, *myFoundation, params);
		DL_ASSERT_EXP(myCooker != nullptr, "Failed to create cooker");

		physx::PxSceneDesc sceneDesc(myPhysicsSDK->getTolerancesScale());
		sceneDesc.gravity = physx::PxVec3(0.f, -9.82f, 0.f);
		if (!sceneDesc.cpuDispatcher)
		{
			physx::PxDefaultCpuDispatcher* mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
			if (!mCpuDispatcher)
			{
				DL_ASSERT("Failed to PxDefaultCpuDispatcherCreate");
			}

			sceneDesc.cpuDispatcher = mCpuDispatcher;
		}

		if (!sceneDesc.filterShader)
		{
			sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		}

		myScene = myPhysicsSDK->createScene(sceneDesc);
		if (!myScene)
		{
			DL_ASSERT("Failed to createScene");
		}


		

		myScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.f);
		myScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.f);

#ifdef _DEBUG
		if (myPhysicsSDK->getPvdConnectionManager())
		{
			myPhysicsSDK->getPvdConnectionManager()->addHandler(*this);
		}

		const char* pvdHostIp = "127.0.0.1";
		int port = 5425;
		unsigned int timeout = 100;
		physx::debugger::PxVisualDebuggerConnectionFlags connectionFlags = physx::debugger::PxVisualDebuggerExt::getAllConnectionFlags();

		myDebugConnection = physx::debugger::PxVisualDebuggerExt::createConnection(myPhysicsSDK->getPvdConnectionManager()
			, pvdHostIp, port, timeout, connectionFlags);

		myPhysicsSDK->getVisualDebugger()->setVisualDebuggerFlag(physx::PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
#endif

		myDefaultMaterial = myPhysicsSDK->createMaterial(0.5, 0.5, 0.5);
		//physx::PxReal d = 0.0f;
		physx::PxTransform pose = physx::PxTransform(physx::PxVec3(0.f, -10.f, 0.f), physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.f, 0.f, 1.f)));
		physx::PxRigidStatic* plane = myPhysicsSDK->createRigidStatic(pose);
		if (!plane)
		{
			DL_ASSERT("Failed to create plane");
		}

		physx::PxShape* shape = plane->createShape(physx::PxPlaneGeometry(), *myDefaultMaterial);
		if (!shape)
		{
			DL_ASSERT("Failed to create shape");
		}

		myScene->addActor(*plane);
		myControllerManager = PxCreateControllerManager(*myScene);

		myControllerManager->setOverlapRecoveryModule(true);
	}


	PhysicsManager::~PhysicsManager()
	{
		myControllerManager->release();
	}

	void PhysicsManager::Update()
	{
		if (!myScene)
		{
			DL_ASSERT("no scene in PhysicsManager");
		}

		myScene->simulate(myTimestep);

		while (!myScene->fetchResults())
		{
			// do something useful..
		}

		//Sleep(16);
	}

	Entity* PhysicsManager::RayCast(const CU::Vector3<float>& aOrigin, const CU::Vector3<float>& aNormalizedDirection, float aMaxRayDistance)
	{
		bool returnValue = false;
		physx::PxVec3 origin;
		origin.x = aOrigin.x;
		origin.y = aOrigin.y;
		origin.z = aOrigin.z;
		physx::PxVec3 unitDirection;
		unitDirection.x = aNormalizedDirection.x;
		unitDirection.y = aNormalizedDirection.y;
		unitDirection.z = aNormalizedDirection.z;
		physx::PxReal maxDistance = aMaxRayDistance;

		physx::PxRaycastHit touches[32];
		physx::PxRaycastBuffer buffer(touches, 32);

		returnValue = myScene->raycast(origin, unitDirection, maxDistance, buffer);
		if (returnValue == true)
		{
			Prism::PhysEntity *ent = nullptr;//static_cast<PhysEntity*>(buffer.touches[0].actor->userData);

			float closestDist = FLT_MAX;
			for (int i = 0; i < buffer.nbTouches; ++i)
			{
				if (buffer.touches[i].distance < closestDist)
				{
					closestDist = buffer.touches[i].distance;
					ent = static_cast<PhysEntity*>(buffer.touches[i].actor->userData);
				}
			}
			if (ent == nullptr)
			{
				return nullptr;
			}
			return ent->GetEntity();
		}
		return nullptr;
		//myScene->Ge = touches[0].actor->getOwnerClient();

		//return returnValue;
	}

	void PhysicsManager::onPvdConnected(physx::debugger::comm::PvdConnection&)
	{
		myPhysicsSDK->getVisualDebugger()->setVisualizeConstraints(true);
		myPhysicsSDK->getVisualDebugger()->setVisualDebuggerFlag(physx::PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
		myPhysicsSDK->getVisualDebugger()->setVisualDebuggerFlag(physx::PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	void PhysicsManager::onPvdDisconnected(physx::debugger::comm::PvdConnection&)
	{
		myDebugConnection->release();
	}

	int PhysicsManager::CreatePlayerController(const CU::Vector3<float>& aStartPosition)
	{
		physx::PxCapsuleControllerDesc controllerDesc;

		//controllerDesc.climbingMode = physx::PxCapsuleClimbingMode::eEASY;
		controllerDesc.height = 1.5f;
		controllerDesc.radius = 0.25f;
		controllerDesc.material = myDefaultMaterial;
		controllerDesc.position = physx::PxExtendedVec3(aStartPosition.x, aStartPosition.y, aStartPosition.z);//fix

		physx::PxController* controller = myControllerManager->createController(controllerDesc);

		return myControllerManager->getNbControllers() - 1;
	}

	void PhysicsManager::Move(int aId, const CU::Vector3<float>& aDirection, float aMinDisplacement, float aDeltaTime)
	{
		physx::PxControllerFilters filter;
		myControllerManager->getController(aId)->move(physx::PxVec3(aDirection.x, aDirection.y, aDirection.z), aMinDisplacement, aDeltaTime, filter, nullptr);
	}

	void PhysicsManager::GetPosition(int aId, CU::Vector3<float>& aPositionOut)
	{
		const physx::PxExtendedVec3& pos = myControllerManager->getController(aId)->getFootPosition();
		aPositionOut.x = pos.x;
		aPositionOut.y = pos.y;
		aPositionOut.z = pos.z;
	}
}