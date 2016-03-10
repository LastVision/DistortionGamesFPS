#include "stdafx.h"
#include "PhysicsManager.h"
#include <ThreadUtilities.h>
#include "PhysicsHelperFunc.h"

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

#include <CommonHelper.h>
#include <pxphysicsapi.h>
#include <PxQueryReport.h>
#include <TimerManager.h>
#include "PhysicsComponentData.h"

#include "wavefront.h"

#define MATERIAL_ID			0x01

namespace Prism
{
	PhysicsManager::PhysicsManager()
		: myPhysicsComponentCallbacks(4096)
#ifdef THREAD_PHYSICS
		, myQuit(false)
		, myLogicDone(false)
		, myPhysicsDone(false)
		, mySwapDone(false)
		, myTimerManager(new CU::TimerManager())
#endif
	{
		myRaycastJobs[0].Init(64);
		myRaycastJobs[1].Init(64);
		myRaycastResults[0].Init(64);
		myRaycastResults[1].Init(64);
		myForceJobs[0].Init(64);
		myForceJobs[1].Init(64);
		myVelocityJobs[0].Init(64);
		myVelocityJobs[1].Init(64);
		myPositionJobs[0].Init(64);
		myPositionJobs[1].Init(64);
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

		myScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.f);
		myScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.f);
		myScene->setFlag(physx::PxSceneFlag::eENABLE_KINEMATIC_PAIRS, true);
		myScene->setFlag(physx::PxSceneFlag::eENABLE_KINEMATIC_STATIC_PAIRS, true);
		myScene->setSimulationEventCallback(this);

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
#ifdef THREAD_PHYSICS
		SAFE_DELETE(myPhysicsThread);
		SAFE_DELETE(myTimerManager);
#endif
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


#ifdef THREAD_PHYSICS
	void PhysicsManager::InitThread()
	{
		myPhysicsThread = new std::thread(&PhysicsManager::ThreadUpdate, this);

		CU::SetThreadName(myPhysicsThread->get_id(), "Physics thread");
	}

	void PhysicsManager::ShutdownThread()
	{
		myQuit = true;
		myLogicDone = true;
		mySwapDone = true;
		myPhysicsThread->join();
	}

	void PhysicsManager::ThreadUpdate()
	{
		while (myQuit == false)
		{
			myTimerManager->Update();
			Update();

			if (myLogicDone == true)
			{
				SetPhysicsDone();
				WaitForSwap();
			}
			myTimerManager->CapFrameRate(60.f);
		}
	}
#endif

	void PhysicsManager::Add(const PhysicsCallbackStruct& aCallbackStruct)
	{
		myPhysicsComponentCallbacks.Add(aCallbackStruct);
	}

	void PhysicsManager::Swap()
	{
		for (int i = 0; i < myPhysicsComponentCallbacks.Size(); ++i)
		{
			myPhysicsComponentCallbacks[i].mySwapOrientationCallback();
		}

		std::swap(myRaycastJobs[0], myRaycastJobs[1]);
		std::swap(myRaycastResults[0], myRaycastResults[1]);
		std::swap(myMoveJobs[0], myMoveJobs[1]);
		std::swap(myForceJobs[0], myForceJobs[1]);
		std::swap(myVelocityJobs[0], myVelocityJobs[1]);
		std::swap(myPositionJobs[0], myPositionJobs[1]);
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


		Move(myMoveJobs[1]);
		const physx::PxExtendedVec3& pos = myControllerManager->getController(myMoveJobs[1].myId)->getFootPosition();
		myPlayerPosition.x = float(pos.x);
		myPlayerPosition.y = float(pos.y);
		myPlayerPosition.z = float(pos.z);

		/*
		for (int i = 0; i < myMoveJobs[0].Size(); ++i)
		{
			Move(myMoveJobs[0][i]);
		}

		if (myMoveJobs[0].Size() > 0)
		{
			const physx::PxExtendedVec3& pos = myControllerManager->getController(myMoveJobs[0][0].myId)->getFootPosition();
			myPlayerPosition.x = float(pos.x);
			myPlayerPosition.y = float(pos.y);
			myPlayerPosition.z = float(pos.z);
		}

		myMoveJobs[0].RemoveAll();
		*/

		for (int i = 0; i < myForceJobs[1].Size(); ++i)
		{
			AddForce(myForceJobs[1][i]);
		}
		myForceJobs[1].RemoveAll();

		for (int i = 0; i < myVelocityJobs[1].Size(); ++i)
		{
			SetVelocity(myVelocityJobs[1][i]);
		}
		myVelocityJobs[1].RemoveAll();

		for (int i = 0; i < myPositionJobs[1].Size(); ++i)
		{
			SetPosition(myPositionJobs[1][i]);
		}
		myPositionJobs[1].RemoveAll();

		for (int i = 0; i < myPhysicsComponentCallbacks.Size(); ++i)
		{
			if (myPhysicsComponentCallbacks[i].myData->myPhysicsType == ePhysics::DYNAMIC)
			{
				myPhysicsComponentCallbacks[i].myUpdateCallback();
			}
		}

		for (int i = 0; i < myRaycastJobs[1].Size(); ++i)
		{
			RayCast(myRaycastJobs[1][i]);
		}

		myRaycastJobs[1].RemoveAll();


		//Sleep(16);
	}

	void PhysicsManager::RayCast(const CU::Vector3<float>& aOrigin, const CU::Vector3<float>& aNormalizedDirection, float aMaxRayDistance, std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&)> aFunctionToCall)
	{
		myRaycastJobs[0].Add(RaycastJob(aOrigin, aNormalizedDirection, aMaxRayDistance, aFunctionToCall));
	}

	void PhysicsManager::onTrigger(physx::PxTriggerPair* somePairs, physx::PxU32 aCount)
	{
		for (physx::PxU32 i = 0; i < aCount; i++)
		{
			const physx::PxTriggerPair& cp = somePairs[i];

			if (somePairs->status == physx::PxPairFlag::Enum::eNOTIFY_TOUCH_FOUND)
			{
				//myTriggerManager.OnTrigger(somePairs->triggerActor->userData, stat_cast<PhysEntity*>(somePairs->triggerActor->userData));
				//if (pairs->triggerActor == myPlayer)
				//{
				//
				//}
				//PhysEntity* ent = static_cast<PhysEntity*>(somePairs->triggerActor->userData);
				//
				//if (ent->myOnTriggerCallBack)
				//{
				//	ent->myOnTriggerCallBack();
				//}
				//TriggerEntity* ent = static_cast<TriggerEntity*>(somePairs->triggerActor->userData);
				//ent;
				//int apa = 5;
				//ent->Collide(somePairs->otherActor->userData);
				//ent->Collide(static_cast<PhysEntity*>(somePairs->otherActor->userData));
				//myTriggerManager.Add(TriggerJob(static_cast<PhysEntity*>(somePairs->triggerActor->userData), static_cast<PhysEntity*>(somePairs->otherActor->userData)));
			}
		}
	}

	void PhysicsManager::RayCast(const RaycastJob& aRaycastJob)
	{
		bool returnValue = false;
		physx::PxVec3 origin(ConvertVector(aRaycastJob.myOrigin));
		physx::PxVec3 unitDirection(ConvertVector(aRaycastJob.myNormalizedDirection));
		physx::PxReal maxDistance = aRaycastJob.myMaxRayDistance;

		physx::PxRaycastHit touches[32];
		physx::PxRaycastBuffer buffer(touches, 32);

		returnValue = myScene->raycast(origin, unitDirection, maxDistance, buffer);
		CU::Vector3<float> hitPosition;
		if (returnValue == true)
		{
			PhysicsComponent* ent = nullptr;//static_cast<PhysEntity*>(buffer.touches[0].actor->userData);
			float closestDist = FLT_MAX;
			for (int i = 0; i < int(buffer.nbTouches); ++i)
			{
				if (buffer.touches[i].distance < closestDist)
				{
					if (buffer.touches[i].distance == 0.f)
					{
						continue;
					}
					closestDist = buffer.touches[i].distance;
					ent = static_cast<PhysicsComponent*>(buffer.touches[i].actor->userData);
					hitPosition.x = buffer.touches[i].position.x;
					hitPosition.y = buffer.touches[i].position.y;
					hitPosition.z = buffer.touches[i].position.z;

				}
			}
			if (ent == nullptr)
			{
				myRaycastResults[1].Add(RaycastResult(nullptr, aRaycastJob.myNormalizedDirection, hitPosition, aRaycastJob.myFunctionToCall));
			}
			else
			{
				myRaycastResults[1].Add(RaycastResult(ent, aRaycastJob.myNormalizedDirection, hitPosition, aRaycastJob.myFunctionToCall));
			}
		}
		else
		{
			myRaycastResults[1].Add(RaycastResult(nullptr, aRaycastJob.myNormalizedDirection, hitPosition, aRaycastJob.myFunctionToCall));
		}
	}

	void PhysicsManager::AddForce(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aDirection, float aMagnitude)
	{
		myForceJobs[0].Add(ForceJob(aDynamicBody, aDirection, aMagnitude));
	}

	void PhysicsManager::AddForce(const ForceJob& aForceJob)
	{
		aForceJob.myDynamicBody->addForce(ConvertVector(aForceJob.myDirection) * aForceJob.myMagnitude, physx::PxForceMode::eVELOCITY_CHANGE);
	}

	void PhysicsManager::SetVelocity(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aVelocity)
	{
		myVelocityJobs[0].Add(VelocityJob(aDynamicBody, aVelocity));
	}

	void PhysicsManager::SetVelocity(const VelocityJob& aVelocityJob)
	{
		aVelocityJob.myDynamicBody->setLinearVelocity(ConvertVector(aVelocityJob.myVelocity));
	}

	void PhysicsManager::SetPosition(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aPosition)
	{
		myPositionJobs[0].Add(PositionJob(aDynamicBody, aPosition));
		}

	void PhysicsManager::SetPosition(const PositionJob& aPositionJob)
	{
		physx::PxTransform pose = physx::PxTransform(
			ConvertVector(aPositionJob.myPosition), physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.f, 0.f, 1.f)));
		aPositionJob.myDynamicBody->setGlobalPose(pose);
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

	int PhysicsManager::CreatePlayerController(const CU::Vector3<float>& aStartPosition, PhysicsComponent* aComponent)
	{
		physx::PxCapsuleControllerDesc controllerDesc;

		//controllerDesc.climbingMode = physx::PxCapsuleClimbingMode::eEASY;
		controllerDesc.height = 1.5f;
		controllerDesc.radius = 0.25f;
		controllerDesc.material = myDefaultMaterial;
		controllerDesc.position = physx::PxExtendedVec3(aStartPosition.x, aStartPosition.y, aStartPosition.z);//fix
		controllerDesc.userData = aComponent;

		myControllerManager->createController(controllerDesc);

		return myControllerManager->getNbControllers() - 1;
	}

	void PhysicsManager::Move(int aId, const CU::Vector3<float>& aDirection, float aMinDisplacement, float aDeltaTime)
	{
		myMoveJobs[0] = MoveJob(aId, aDirection, aMinDisplacement, aDeltaTime);
	}
		
	void PhysicsManager::Move(const MoveJob& aMoveJob)
	{
		physx::PxControllerFilters filter;
		myControllerManager->getController(aMoveJob.myId)->move(
			ConvertVector(aMoveJob.myDirection), aMoveJob.myMinDisplacement, aMoveJob.myDeltaTime, filter, nullptr);
	}

	void PhysicsManager::UpdateOrientation(physx::PxRigidDynamic* aDynamicBody, physx::PxShape** aShape, float* aThread4x4)
	{
		physx::PxU32 nShapes = aDynamicBody->getNbShapes();
		aDynamicBody->getShapes(aShape, nShapes);

		physx::PxTransform pT = physx::PxShapeExt::getGlobalPose(*aShape[0], *aDynamicBody);
		physx::PxTransform graphicsTransform(pT.p, pT.q);
		physx::PxMat33 m = physx::PxMat33(graphicsTransform.q);

		aThread4x4[0] = m(0, 0);
		aThread4x4[1] = m(1, 0);
		aThread4x4[2] = m(2, 0);
		aThread4x4[3] = 0;

		aThread4x4[4] = m(0, 1);
		aThread4x4[5] = m(1, 1);
		aThread4x4[6] = m(2, 1);
		aThread4x4[7] = 0;

		aThread4x4[8] = m(0, 2);
		aThread4x4[9] = m(1, 2);
		aThread4x4[10] = m(2, 2);
		aThread4x4[11] = 0;

		aThread4x4[12] = graphicsTransform.p.x;
		aThread4x4[13] = graphicsTransform.p.y;
		aThread4x4[14] = graphicsTransform.p.z;
		aThread4x4[15] = 1;
	}

	bool PhysicsManager::GetAllowedToJump(int aId)
	{
		physx::PxControllerState state;
		myControllerManager->getController(aId)->getState(state);
		return state.touchedActor != nullptr;
	}

	void PhysicsManager::SetPosition(int aId, const CU::Vector3<float>& aPosition)
	{
		DL_ASSERT("Not impl. yet");
		//physx::PxControllerFilters filter;
		//myControllerManager->getController(aId)->setFootPosition(physx::PxExtendedVec3(aPosition.x, aPosition.y, aPosition.z));
	}

	void PhysicsManager::GetPosition(int aId, CU::Vector3<float>& aPositionOut)
	{
		aPositionOut = myPlayerPosition;
	}

	void PhysicsManager::Create(PhysicsComponent* aComponent, const PhysicsCallbackStruct& aPhysData
		, float* aOrientation, const std::string& aFBXPath
		, physx::PxRigidDynamic** aDynamicBodyOut, physx::PxRigidStatic** aStaticBodyOut
		, physx::PxShape*** someShapesOut)
	{
		physx::PxPhysics* core = GetCore();
		physx::PxMaterial* material = core->createMaterial(0.5, 0.5, 0.5);

		physx::PxReal density = 1.f;

		//Use 4x4float here instead of orientation
		physx::PxMat44 matrix(aOrientation);
		physx::PxTransform transform(matrix);

		*aStaticBodyOut = nullptr;
		*aDynamicBodyOut = nullptr;

		if (aPhysData.myData->myPhysicsType == ePhysics::STATIC)
		{
			physx::PxTriangleMesh* mesh = GetPhysMesh(aFBXPath);

			*aStaticBodyOut = core->createRigidStatic(transform);
			(*aStaticBodyOut)->createShape(physx::PxTriangleMeshGeometry(mesh), *material);
			(*aStaticBodyOut)->setName("Tjohej");
			(*aStaticBodyOut)->userData = aComponent;
			GetScene()->addActor(*(*aStaticBodyOut));
		}
		else if (aPhysData.myData->myPhysicsType == ePhysics::DYNAMIC)
		{
			physx::PxVec3 dimensions(
				aPhysData.myData->myPhysicsMax.x - aPhysData.myData->myPhysicsMin.x
				, aPhysData.myData->myPhysicsMax.y - aPhysData.myData->myPhysicsMin.y
				, aPhysData.myData->myPhysicsMax.z - aPhysData.myData->myPhysicsMin.z);
			physx::PxBoxGeometry geometry(dimensions / 2.f);
			*aDynamicBodyOut = physx::PxCreateDynamic(*core, transform, geometry, *material, density);
			(*aDynamicBodyOut)->setAngularDamping(0.75);
			(*aDynamicBodyOut)->setLinearVelocity(physx::PxVec3(0, 0, 0));
			(*aDynamicBodyOut)->userData = aComponent;
			GetScene()->addActor(*(*aDynamicBodyOut));

			physx::PxU32 nShapes = (*aDynamicBodyOut)->getNbShapes();
			
			*someShapesOut = new physx::PxShape*[nShapes];
		}
		else if (aPhysData.myData->myPhysicsType == ePhysics::PHANTOM)
		{
			physx::PxVec3 dimensions(
				aPhysData.myData->myPhysicsMax.x - aPhysData.myData->myPhysicsMin.x
				, aPhysData.myData->myPhysicsMax.y - aPhysData.myData->myPhysicsMin.y
				, aPhysData.myData->myPhysicsMax.z - aPhysData.myData->myPhysicsMin.z);
			physx::PxBoxGeometry geometry(dimensions / 2.f);
			*aStaticBodyOut = physx::PxCreateStatic(*core, transform, geometry, *material);
			(*aStaticBodyOut)->userData = aComponent;
			(*aStaticBodyOut)->setName("Phantom");

			physx::PxU32 nShapes = (*aStaticBodyOut)->getNbShapes();
			*someShapesOut = new physx::PxShape*[nShapes];

			physx::PxShape* treasureShape;
			(*aStaticBodyOut)->getShapes(&treasureShape, 1.f);

			treasureShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
			treasureShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
			GetScene()->addActor(*(*aStaticBodyOut));
		}

		myPhysicsComponentCallbacks.Add(aPhysData);
	}

	void PhysicsManager::Remove(physx::PxRigidDynamic* aDynamic, const PhysicsComponentData& aData)
	{
		GetScene()->removeActor(*aDynamic);
		for (int i = 0; i < myPhysicsComponentCallbacks.Size(); ++i)
		{
			if (myPhysicsComponentCallbacks[i].myData == &aData)
			{
				myPhysicsComponentCallbacks.RemoveCyclicAtIndex(i);
			}
		}
	}

	void PhysicsManager::Remove(physx::PxRigidStatic* aStatic, const PhysicsComponentData& aData)
	{
		GetScene()->removeActor(*aStatic);
		for (int i = 0; i < myPhysicsComponentCallbacks.Size(); ++i)
		{
			if (myPhysicsComponentCallbacks[i].myData == &aData)
			{
				myPhysicsComponentCallbacks.RemoveCyclicAtIndex(i);
			}
		}
	}

	physx::PxTriangleMesh* PhysicsManager::GetPhysMesh(const std::string& aFBXPath)
	{
		std::string objPath(aFBXPath);
		std::string cowPath(aFBXPath);

		objPath[aFBXPath.size() - 3] = 'o';
		objPath[aFBXPath.size() - 2] = 'b';
		objPath[aFBXPath.size() - 1] = 'j';

		cowPath[aFBXPath.size() - 3] = 'c';
		cowPath[aFBXPath.size() - 2] = 'o';
		cowPath[aFBXPath.size() - 1] = 'w';

		cowPath = CU::GetGeneratedDataFolderFilePath(aFBXPath, "cow");

		physx::PxTriangleMesh* mesh = nullptr;
		WavefrontObj wfo;

		bool ok;

		if (CU::FileExists(cowPath) == false)
		{

			if (!wfo.loadObj(objPath.c_str(), false))
			{
				DL_ASSERT(CU::Concatenate("Error loading file: %s", objPath.c_str()));
			}

			physx::PxTriangleMeshDesc meshDesc;
			meshDesc.points.count = wfo.mVertexCount;
			meshDesc.triangles.count = wfo.mTriCount;
			meshDesc.points.stride = sizeof(float) * 3;
			meshDesc.triangles.stride = sizeof(int) * 3;
			meshDesc.points.data = wfo.mVertices;
			meshDesc.triangles.data = wfo.mIndices;

			{
				physx::PxDefaultFileOutputStream stream(cowPath.c_str());
				ok = GetCooker()->cookTriangleMesh(meshDesc, stream);
			}
		}
		else
		{
			ok = true;
		}
		if (ok)
		{
			physx::PxDefaultFileInputData stream(cowPath.c_str());
			mesh = GetCore()->createTriangleMesh(stream);
		}

		return mesh;
	}

	void PhysicsManager::EndFrame()
	{
		for (int i = 0; i < myRaycastResults[0].Size(); ++i)
		{
			myRaycastResults[0][i].myFunctionToCall(myRaycastResults[0][i].myPhysicsComponent, myRaycastResults[0][i].myDirection, myRaycastResults[0][i].myHitPosition);
		}

		myRaycastResults[0].RemoveAll();
	}
}