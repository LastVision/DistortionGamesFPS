#pragma once
#include <functional>
#include <Vector.h>

class Entity;

namespace physx
{
	class PxRigidDynamic;
}

namespace Prism
{
	class PhysicsManager;
	class PhysicsInterface
	{
	public:
		static void Create();
		static void Destroy();
		static PhysicsInterface* GetInstance();
#ifdef THREAD_PHYSICS
		void InitThread();
		void ShutdownThread();
#endif
		// Requires PhysX includes!!
		PhysicsManager* GetManager() const;

		void EndFrame();

		void RayCast(const CU::Vector3<float>& aOrigin, const CU::Vector3<float>& aNormalizedDirection, float aMaxRayDistance, std::function<void(Entity*, const CU::Vector3<float>&, const CU::Vector3<float>&)> aFunctionToCall);
		void AddForce(physx::PxRigidDynamic* aDynamicBody, const CU::Vector3<float>& aDirection, float aMagnitude);

		int CreatePlayerController(const CU::Vector3<float>& aStartPosition);
		void Move(int aId, const CU::Vector3<float>& aDirection, float aMinDisplacement, float aDeltaTime);
		bool GetAllowedToJump(int aId);
		void SetPosition(int aId, const CU::Vector3<float>& aPosition);
		void GetPosition(int aId, CU::Vector3<float>& aPositionOut);

	private:
		PhysicsInterface();
		~PhysicsInterface();

		PhysicsManager* myManager;
		static PhysicsInterface* myInstance;
	};
}