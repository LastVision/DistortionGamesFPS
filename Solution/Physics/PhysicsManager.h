#pragma once

#include <PxPhysics.h>
#include <extensions/pxdefaulterrorcallback.h>
#include <extensions/pxdefaultallocator.h>
#include <GrowingArray.h>
#include <pvd/PxVisualDebugger.h>
#include <physxvisualdebuggersdk/PvdConnection.h>
#include <characterkinematic\PxControllerManager.h>
#include <cooking\PxCooking.h>
#include <Vector.h>

class Entity;

namespace CU
{
	class TimerManager;
}

namespace physx
{
	class PxDefaultCpuDispatcher;
	class PxRigidDynamic;
}

namespace Prism
{
	class PhysEntity;

	class PhysicsManager : public physx::debugger::comm::PvdConnectionHandler
	{
	public:
		PhysicsManager();
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
		void Add(PhysEntity* aPhysEntity);
		void Swap();

		void Update();

		void RayCast(const CU::Vector3<float>& aOrigin, const CU::Vector3<float>& aNormalizedDirection, float aMaxRayDistance, std::function<void(Entity*, const CU::Vector3<float>&, const CU::Vector3<float>&)> aFunctionToCall);
		void AddForce(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aDirection, float aMagnitude);
		void SetVelocity(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aVelocity);
		void SetPosition(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aPosition);

		physx::PxPhysics* GetCore(){ return myPhysicsSDK; }
		physx::PxScene* GetScene(){ return myScene; }
		physx::PxCooking* GetCooker(){ return myCooker; }

		int CreatePlayerController(const CU::Vector3<float>& aStartPosition);
		void Move(int aId, const CU::Vector3<float>& aDirection, float aMinDisplacement, float aDeltaTime);
		bool GetAllowedToJump(int aId);
		void SetPosition(int aId, const CU::Vector3<float>& aPosition);
		void GetPosition(int aId, CU::Vector3<float>& aPositionOut);

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
			RaycastJob(const CU::Vector3<float>& aOrigin, const CU::Vector3<float>& aNormalizedDirection, float aMaxRayDistance, std::function<void(Entity*, const CU::Vector3<float>&, const CU::Vector3<float>&)> aFunctionToCall)
				: myOrigin(aOrigin)
				, myNormalizedDirection(aNormalizedDirection)
				, myMaxRayDistance(aMaxRayDistance)
				, myFunctionToCall(aFunctionToCall)
			{}
			CU::Vector3<float> myOrigin;
			CU::Vector3<float> myNormalizedDirection;
			float myMaxRayDistance;
			std::function<void(Entity*, const CU::Vector3<float>&, const CU::Vector3<float>&)> myFunctionToCall;
		};
		void RayCast(const RaycastJob& aRaycastJob);
		CU::GrowingArray<RaycastJob> myRaycastJobs[2];

		struct RaycastResult
		{
			RaycastResult() {}
			RaycastResult(Entity* anEntity, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, std::function<void(Entity*, const CU::Vector3<float>&, const CU::Vector3<float>&)> aFunctionToCall)
				: myEntity(anEntity)
				, myDirection(aDirection)
				, myHitPosition(aHitPosition)
				, myFunctionToCall(aFunctionToCall)
			{}
			Entity* myEntity;
			CU::Vector3<float> myDirection;
			CU::Vector3<float> myHitPosition;
			std::function<void(Entity*, const CU::Vector3<float>&, const CU::Vector3<float>&)> myFunctionToCall;
		};
		CU::GrowingArray<RaycastResult> myRaycastResults[2];

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

		struct PositionJob
		{
			PositionJob()
				: myDynamicBody(nullptr)
			{}

			PositionJob(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aPosition)
				: myDynamicBody(aDynamicBody)
				, myPosition(aPosition)
			{}

			physx::PxRigidDynamic* myDynamicBody;
			CU::Vector3<float> myPosition;
		};
		CU::GrowingArray<PositionJob> myPositionJobs[2];
		void SetPosition(const PositionJob& aPositionJob);

		CU::GrowingArray<PhysEntity*> myPhysEntities;

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