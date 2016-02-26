#pragma once

#include <Vector.h>

class Entity;

namespace Prism
{
	class PhysicsManager;
	class PhysicsInterface
	{
	public:
		static void Create();
		static void Destroy();
		static PhysicsInterface* GetInstance();

		// Requires PhysX includes!!
		PhysicsManager* GetManager() const;

		void Update();

		Entity* RayCast(const CU::Vector3<float>& aOrigin, const CU::Vector3<float>& aNormalizedDirection, float aMaxRayDistance);

	private:
		PhysicsInterface();
		~PhysicsInterface();

		PhysicsManager* myManager;
		static PhysicsInterface* myInstance;
	};
}