#pragma once

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

	private:
		PhysicsInterface();
		~PhysicsInterface();

		PhysicsManager* myManager;
		static PhysicsInterface* myInstance;
	};
}