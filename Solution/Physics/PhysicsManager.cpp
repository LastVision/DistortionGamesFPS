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
#else
#pragma comment(lib, "PhysX\\vc12win32\\PhysX3_x86.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PhysX3Common_x86.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PhysX3Cooking_x86.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PhysXProfileSDK.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PxTask.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PhysX3Extensions.lib")
#pragma comment(lib, "PhysX\\vc12win32\\PhysXVisualDebuggerSDK.lib")
#endif

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

		physx::PxMaterial* material = myPhysicsSDK->createMaterial(0.5, 0.5, 0.5);
		//physx::PxReal d = 0.0f;
		physx::PxTransform pose = physx::PxTransform(physx::PxVec3(0.f, -10.f, 0.f), physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.f, 0.f, 1.f)));
		physx::PxRigidStatic* plane = myPhysicsSDK->createRigidStatic(pose);
		if (!plane)
		{
			DL_ASSERT("Failed to create plane");
		}

		physx::PxShape* shape = plane->createShape(physx::PxPlaneGeometry(), *material);
		if (!shape)
		{
			DL_ASSERT("Failed to create shape");
		}

		myScene->addActor(*plane);
	}


	PhysicsManager::~PhysicsManager()
	{
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

	bool PhysicsManager::RayCast(const CU::Vector3<float>& aOrigin, const CU::Vector3<float>& aNormalizedDirection, float aMaxRayDistance)
	{
		physx::PxVec3 origin;
		origin.x = aOrigin.x;
		origin.y = aOrigin.y;
		origin.z = aOrigin.z;
		physx::PxVec3 unitDirection;
		unitDirection.x = aNormalizedDirection.x;
		unitDirection.y = aNormalizedDirection.y;
		unitDirection.z = aNormalizedDirection.z;
		physx::PxReal maxDistance = aMaxRayDistance;

		physx::PxRaycastBuffer buffer;

		return myScene->raycast(origin, unitDirection, maxDistance, buffer);

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
}