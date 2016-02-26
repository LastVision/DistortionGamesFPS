#pragma once

#include <PxPhysics.h>
#include <extensions/pxdefaulterrorcallback.h>
#include <extensions/pxdefaultallocator.h>
#include <pvd/PxVisualDebugger.h>
#include <physxvisualdebuggersdk/PvdConnection.h>
#include <cooking\PxCooking.h>
#include <Vector.h>

class Entity;

namespace Prism
{
	class PhysicsManager : public physx::debugger::comm::PvdConnectionHandler
	{
	public:
		PhysicsManager();
		~PhysicsManager();

		void Update();

		Entity* RayCast(const CU::Vector3<float>& aOrigin, const CU::Vector3<float>& aNormalizedDirection, float aMaxRayDistance);

		physx::PxPhysics* GetCore(){ return myPhysicsSDK; }
		physx::PxScene* GetScene(){ return myScene; }
		physx::PxCooking* GetCooker(){ return myCooker; }

	private:
		void onPvdSendClassDescriptions(physx::debugger::comm::PvdConnection&) override{}
		void onPvdConnected(physx::debugger::comm::PvdConnection& connection) override;
		void onPvdDisconnected(physx::debugger::comm::PvdConnection& connection) override;

		physx::PxScene* myScene;
		physx::PxReal myTimestep;
		physx::PxFoundation* myFoundation;
		physx::PxDefaultErrorCallback myDefaultErrorCallback;
		physx::PxDefaultAllocator myDefaultAllocatorCallback;
		physx::PxProfileZoneManager* myProfileZoneManager;
		physx::PxPhysics* myPhysicsSDK;
		physx::debugger::comm::PvdConnection* myDebugConnection;
		physx::PxCooking* myCooker;
	};
}