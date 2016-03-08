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
		myRaycastJobs[0].Init(64);
		myRaycastJobs[1].Init(64);
		myRaycastResults[0].Init(64);
		myRaycastResults[1].Init(64);
		myTimestep = 1.f / 60.f;
		
		myFoundation = PxCreateFoundation(0x03030300, myDefaultAllocatorCallback, myDefaultErrorCallback);

		myProfileZoneManager = nullptr;
		//myProfileZoneManager = &physx::PxProfileZoneManager::createProfileZoneManager(myFoundation);

		bool recordMemoryAllocations = true;
		myPhysicsSDK = PxCreatePhysics(0x03030300, *myFoundation, physx::PxTolerancesScale(),
			recordMemoryAllocations, nullptr);

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
		
		myCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(0);

		if (!sceneDesc.cpuDispatcher)
		{
			if (!myCpuDispatcher)
			{
				DL_ASSERT("Failed to PxDefaultCpuDispatcherCreate");
			}

			sceneDesc.cpuDispatcher = myCpuDispatcher;
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

		//PxScene.setSimulationEventCallback() PxSceneDesc.simulationEventCallback
		
		myScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.f);
		myScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.f);
		myScene->setFlag(physx::PxSceneFlag::eENABLE_KINEMATIC_PAIRS, true);
		myScene->setFlag(physx::PxSceneFlag::eENABLE_KINEMATIC_STATIC_PAIRS, true);
		//myScene->setSimulationEventCallback(this);

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
		if (myDebugConnection != nullptr)
		{
			//myDebugConnection->release();
		}
		myControllerManager->release();
		myScene->release();
		myCpuDispatcher->release();
		myCooker->release();
		PxCloseExtensions();
		myPhysicsSDK->release();
		myFoundation->release();
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

		for (int i = 0; i < myRaycastJobs[0].Size(); ++i)
		{
			RayCast(myRaycastJobs[0][i]);
		}

		myRaycastJobs[0].RemoveAll();

		for (int i = 0; i < myRaycastResults[0].Size(); ++i)
		{
			myRaycastResults[0][i].myFunctionToCall(myRaycastResults[0][i].myEntity, myRaycastResults[0][i].myDirection);
		}

		myRaycastResults[0].RemoveAll();
		//Sleep(16);
	}

	void PhysicsManager::RayCast(const CU::Vector3<float>& aOrigin, const CU::Vector3<float>& aNormalizedDirection, float aMaxRayDistance, std::function<void(Entity*, const CU::Vector3<float>&)> aFunctionToCall)
	{
		myRaycastJobs[0].Add(RaycastJob(aOrigin, aNormalizedDirection, aMaxRayDistance, aFunctionToCall));
	}

	void PhysicsManager::RayCast(const RaycastJob& aRaycastJob)
	{
		bool returnValue = false;
		physx::PxVec3 origin;
		origin.x = aRaycastJob.myOrigin.x;
		origin.y = aRaycastJob.myOrigin.y;
		origin.z = aRaycastJob.myOrigin.z;
		physx::PxVec3 unitDirection;
		unitDirection.x = aRaycastJob.myNormalizedDirection.x;
		unitDirection.y = aRaycastJob.myNormalizedDirection.y;
		unitDirection.z = aRaycastJob.myNormalizedDirection.z;
		physx::PxReal maxDistance = aRaycastJob.myMaxRayDistance;

		physx::PxRaycastHit touches[32];
		physx::PxRaycastBuffer buffer(touches, 32);

		returnValue = myScene->raycast(origin, unitDirection, maxDistance, buffer);
		if (returnValue == true)
		{
			Prism::PhysEntity* ent = nullptr;//static_cast<PhysEntity*>(buffer.touches[0].actor->userData);

			float closestDist = FLT_MAX;
			for (int i = 0; i < buffer.nbTouches; ++i)
			{
				if (buffer.touches[i].distance < closestDist)
				{
					if (buffer.touches[i].distance == 0.f)
					{
						continue;
					}
					closestDist = buffer.touches[i].distance;
					ent = static_cast<PhysEntity*>(buffer.touches[i].actor->userData);
				}
			}
			if (ent == nullptr)
			{
				myRaycastResults[0].Add(RaycastResult(nullptr, aRaycastJob.myNormalizedDirection, aRaycastJob.myFunctionToCall));
			}
			myRaycastResults[0].Add(RaycastResult(ent->GetEntity(), aRaycastJob.myNormalizedDirection, aRaycastJob.myFunctionToCall));
		}
		myRaycastResults[0].Add(RaycastResult(nullptr, aRaycastJob.myNormalizedDirection, aRaycastJob.myFunctionToCall));
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

	bool PhysicsManager::GetAllowedToJump(int aId)
	{
		physx::PxControllerState state;
		myControllerManager->getController(aId)->getState(state);
		return state.touchedActor != nullptr;
	}

	void PhysicsManager::SetPosition(int aId, const CU::Vector3<float>& aPosition)
	{
		physx::PxControllerFilters filter;
		myControllerManager->getController(aId)->setFootPosition(physx::PxExtendedVec3(aPosition.x, aPosition.y, aPosition.z));
	}

	void PhysicsManager::GetPosition(int aId, CU::Vector3<float>& aPositionOut)
	{
		const physx::PxExtendedVec3& pos = myControllerManager->getController(aId)->getFootPosition();
		aPositionOut.x = pos.x;
		aPositionOut.y = pos.y;
		aPositionOut.z = pos.z;
	}
}