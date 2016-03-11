#pragma once

#include <PxPhysics.h>
#include <extensions/pxdefaulterrorcallback.h>
#include <extensions/pxdefaultallocator.h>
#include <GrowingArray.h>
#include <pvd/PxVisualDebugger.h>
#include <physxvisualdebuggersdk/PvdConnection.h>
#include "PhysicsCallbackStruct.h"
#include <PxSimulationEventCallback.h>
#include <characterkinematic\PxControllerManager.h>
#include <cooking\PxCooking.h>
#include <Vector.h>
#include <Matrix44.h>

class Entity;
class PhysicsComponent;
struct PhysicsComponentData;
namespace CU
{
	class TimerManager;
}

namespace physx
{
	class PxDefaultCpuDispatcher;
	class PxRigidDynamic;
	class PxTriangleMesh;
	class PxActor;
}

class PhysicsComponent;

namespace Prism
{

	class PhysicsManager : public physx::debugger::comm::PvdConnectionHandler, public physx::PxSimulationEventCallback
	{
	public:
		PhysicsManager(std::function<void(PhysicsComponent*, PhysicsComponent*)> anOnTriggerCallback);
		~PhysicsManager();

#ifdef THREAD_PHYSICS
		void InitThread();
		void ShutdownThread();

		void WaitForLogic();
		void WaitForPhysics();
		void WaitForSwap();
		void SetLogicDone();
		void SetPhysicsDone();
		void SetSwapDone();
#endif

		void EndFrame();
		void Add(const PhysicsCallbackStruct& aCallbackStruct);
		void Swap();

		void Update();

		void RayCast(const CU::Vector3<float>& aOrigin, const CU::Vector3<float>& aNormalizedDirection, float aMaxRayDistance, std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&)> aFunctionToCall);
		void AddForce(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aDirection, float aMagnitude);
		void SetVelocity(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aVelocity);
		void TeleportToPosition(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aPosition);
		void MoveToPosition(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aPosition);

		void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) override {};
		void onWake(physx::PxActor**, physx::PxU32) override {};
		void onSleep(physx::PxActor**, physx::PxU32) override {};
		void onContact(const physx::PxContactPairHeader&, const physx::PxContactPair*, physx::PxU32) override {};
		void onTrigger(physx::PxTriggerPair*, physx::PxU32) override;

		physx::PxPhysics* GetCore(){ return myPhysicsSDK; }
		physx::PxScene* GetScene(){ return myScene; }
		physx::PxCooking* GetCooker(){ return myCooker; }

		int CreatePlayerController(const CU::Vector3<float>& aStartPosition, PhysicsComponent* aComponent);
		void Move(int aId, const CU::Vector3<float>& aDirection, float aMinDisplacement, float aDeltaTime);
		void UpdateOrientation(physx::PxRigidDynamic* aDynamicBody, physx::PxShape** aShape, float* aThread4x4);
		bool GetAllowedToJump(int aId);
		void SetPosition(int aId, const CU::Vector3<float>& aPosition);
		void GetPosition(int aId, CU::Vector3<float>& aPositionOut);

		void Create(PhysicsComponent* aComponent, const PhysicsCallbackStruct& aCallbackStruct
			, float* aOrientation, const std::string& aFBXPath
			, physx::PxRigidDynamic** aDynamicBodyOut, physx::PxRigidStatic** aStaticBodyOut
			, physx::PxShape*** someShapesOut);
		void Remove(physx::PxRigidDynamic* aDynamic, const PhysicsComponentData& aData);
		void Remove(physx::PxRigidStatic* aStatic, const PhysicsComponentData& aData);

	private:
#ifdef THREAD_PHYSICS
		CU::TimerManager* myTimerManager;
		void ThreadUpdate();
		std::thread* myPhysicsThread;
		volatile bool myQuit;
		volatile bool myLogicDone;
		volatile bool myPhysicsDone;
		volatile bool mySwapDone;
#endif
		struct RaycastJob
		{
			RaycastJob() {}
			RaycastJob(const CU::Vector3<float>& aOrigin, const CU::Vector3<float>& aNormalizedDirection, float aMaxRayDistance, std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&)> aFunctionToCall)
				: myOrigin(aOrigin)
				, myNormalizedDirection(aNormalizedDirection)
				, myMaxRayDistance(aMaxRayDistance)
				, myFunctionToCall(aFunctionToCall)
			{}
			CU::Vector3<float> myOrigin;
			CU::Vector3<float> myNormalizedDirection;
			float myMaxRayDistance;
			std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&)> myFunctionToCall;
		};
		void RayCast(const RaycastJob& aRaycastJob);
		CU::GrowingArray<RaycastJob> myRaycastJobs[2];

		struct RaycastResult
		{
			RaycastResult() {}
			RaycastResult(PhysicsComponent* aPhysicsComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&)> aFunctionToCall)
				: myPhysicsComponent(aPhysicsComponent)
				, myDirection(aDirection)
				, myHitPosition(aHitPosition)
				, myFunctionToCall(aFunctionToCall)
			{}
			PhysicsComponent* myPhysicsComponent;
			CU::Vector3<float> myDirection;
			CU::Vector3<float> myHitPosition;
			std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&)> myFunctionToCall;
		};
		CU::GrowingArray<RaycastResult> myRaycastResults[2];

		struct OnTriggerResult
		{
			OnTriggerResult() {}
			OnTriggerResult(PhysicsComponent* aFirstPhysicsComponent, PhysicsComponent* aSecondPhysicsComponent)
				: myFirstPhysicsComponent(aFirstPhysicsComponent)
				, mySecondPhysicsComponent(aSecondPhysicsComponent)
			{}
			PhysicsComponent* myFirstPhysicsComponent;
			PhysicsComponent* mySecondPhysicsComponent;
			//std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&)> myFunctionToCall;
		};
		CU::GrowingArray<OnTriggerResult> myOnTriggerResults[2];

		std::function<void(PhysicsComponent*, PhysicsComponent*)> myOnTriggerCallback;

		struct MoveJob
		{
			MoveJob()
				: myId(0)
				, myMinDisplacement(0.f)
				, myDeltaTime(1.f/60.f)
			{}
			MoveJob(int aId, const CU::Vector3<float>& aDirection, float aMinDisplacement, float aDeltaTime)
				: myId(aId)
				, myDirection(aDirection)
				, myMinDisplacement(aMinDisplacement)
				, myDeltaTime(aDeltaTime)
			{}
			int myId;
			CU::Vector3<float> myDirection;
			float myMinDisplacement;
			float myDeltaTime;
		};
		MoveJob myMoveJobs[2];
		void Move(const MoveJob& aMoveJob);

		struct ForceJob
		{
			ForceJob() 
				: myDynamicBody(nullptr)
				, myMagnitude(0.f)
			{}

			ForceJob(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aDirection, float aMagnitude)
				: myDynamicBody(aDynamicBody)
				, myDirection(aDirection)
				, myMagnitude(aMagnitude)
			{}

			physx::PxRigidDynamic* myDynamicBody;
			CU::Vector3<float> myDirection;
			float myMagnitude;
		};
		CU::GrowingArray<ForceJob> myForceJobs[2];
		void AddForce(const ForceJob& aForceJob);

		struct VelocityJob
		{
			VelocityJob()
				: myDynamicBody(nullptr)
			{}

			VelocityJob(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aVelocity)
				: myDynamicBody(aDynamicBody)
				, myVelocity(aVelocity)
			{}

			physx::PxRigidDynamic* myDynamicBody;
			CU::Vector3<float> myVelocity;
		};
		CU::GrowingArray<VelocityJob> myVelocityJobs[2];
		void SetVelocity(const VelocityJob& aVelocityJob);

		enum class PositionJobType
		{
			NONE,
			TELEPORT,
			MOVE,
		};
		struct PositionJob
		{
			PositionJob()
				: myDynamicBody(nullptr)
				, myType(PositionJobType::NONE)
			{}

			PositionJob(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aPosition, PositionJobType aType)
				: myDynamicBody(aDynamicBody)
				, myPosition(aPosition)
				, myType(aType)
			{}

			physx::PxRigidDynamic* myDynamicBody;
			CU::Vector3<float> myPosition;
			PositionJobType myType;
		};
		CU::GrowingArray<PositionJob> myPositionJobs[2];
		void SetPosition(const PositionJob& aPositionJob);

		CU::GrowingArray<PhysicsCallbackStruct> myPhysicsComponentCallbacks;

		physx::PxTriangleMesh* GetPhysMesh(const std::string& aFBXPath);
		void onPvdSendClassDescriptions(physx::debugger::comm::PvdConnection&) override{}
		void onPvdConnected(physx::debugger::comm::PvdConnection& connection) override;
		void onPvdDisconnected(physx::debugger::comm::PvdConnection& connection) override;

		physx::PxScene* myScene;
		physx::PxMaterial* myDefaultMaterial;
		physx::PxControllerManager* myControllerManager;
		physx::PxReal myTimestep;
		physx::PxFoundation* myFoundation;
		physx::PxDefaultErrorCallback myDefaultErrorCallback;
		physx::PxDefaultAllocator myDefaultAllocatorCallback;
		physx::PxProfileZoneManager* myProfileZoneManager;
		physx::PxPhysics* myPhysicsSDK;
		physx::PxDefaultCpuDispatcher* myCpuDispatcher;
		physx::debugger::comm::PvdConnection* myDebugConnection;
		physx::PxCooking* myCooker;

		CU::Vector3<float> myPlayerPosition;

		CU::GrowingArray<physx::PxActor*> myActorsToRemove[2];
	};

#ifdef THREAD_PHYSICS
	inline void PhysicsManager::WaitForLogic()
	{
		while (myLogicDone == false);
		myLogicDone = false;
	}

	inline void PhysicsManager::WaitForPhysics()
	{
		while (myPhysicsDone == false);
		myPhysicsDone = false;
	}

	inline void PhysicsManager::WaitForSwap()
	{
		while (mySwapDone == false);
		mySwapDone = false;
	}

	inline void PhysicsManager::SetLogicDone()
	{
		myLogicDone = true;
	}

	inline void PhysicsManager::SetPhysicsDone()
	{
		myPhysicsDone = true;
	}

	inline void PhysicsManager::SetSwapDone()
	{
		mySwapDone = true;
	}
#endif
}