#pragma once
#include <GrowingArray.h>


namespace Prism
{
	class Instance;

	class RoomManager
	{
	public:
		RoomManager();
		~RoomManager();

		void Add(Instance* anInstance);
		void Remove(Instance* anInstance);

		const CU::GrowingArray<Instance*>& GetActiveInstances(const Camera& aCamera);

	private:
		CU::GrowingArray<Instance*> myInstances;
		CU::GrowingArray<Instance*> myActiveInstances;
	};
}