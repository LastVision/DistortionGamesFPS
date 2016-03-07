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
#endif
		void Add(PhysEntity* aPhysEntity);
		void SwapOrientations();

		void Update();

		void RayCast(const CU::Vector3<float>& aOrigin, const CU::Vector3<float>& aNormalizedDirection, float aMaxRayDistance, std::function<void(Entity*, const CU::Vector3<float>&)> aFunctionToCall);

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
#endif
		struct RaycastJob
		{
			RaycastJob() {}
			RaycastJob(const CU::Vector3<float>& aOrigin, const CU::Vector3<float>& aNormalizedDirection, float aMaxRayDistance, std::function<void(Entity*, const CU::Vector3<float>&)> aFunctionToCall)
				: myOrigin(aOrigin)
				, myNormalizedDirection(aNormalizedDirection)
				, myMaxRayDistance(aMaxRayDistance)
				, myFunctionToCall(aFunctionToCall)
			{}
			CU::Vector3<float> myOrigin;
			CU::Vector3<float> myNormalizedDirection;
			float myMaxRayDistance;
			std::function<void(Entity*, const CU::Vector3<float>&)> myFunctionToCall;
		};
		void RayCast(const RaycastJob& aRaycastJob);
		CU::GrowingArray<RaycastJob> myRaycastJobs[2];

		struct RaycastResult
		{
			RaycastResult() {}
			RaycastResult(Entity* anEntity, const CU::Vector3<float>& aDirection, std::function<void(Entity*, const CU::Vector3<float>&)> aFunctionToCall)
				: myEntity(anEntity)
				, myDirection(aDirection)
				, myFunctionToCall(aFunctionToCall)
			{}
			Entity* myEntity;
			CU::Vector3<float> myDirection;
			std::function<void(Entity*, const CU::Vector3<float>&)> myFunctionToCall;
		};
		CU::GrowingArray<RaycastResult> myRaycastResults[2];

		struct MoveJob
		{
			MoveJob() {}
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
		CU::GrowingArray<MoveJob> myMoveJobs[2];
		void Move(const MoveJob& aMoveJob);

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
}