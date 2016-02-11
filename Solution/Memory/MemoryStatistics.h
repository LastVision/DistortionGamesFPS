#pragma once

namespace Prism
{
	struct MemoryStatistics
	{
		MemoryStatistics()
			: myAccumulatedMemoryAllocated(0)
			, myAccumulatedNumberOfAllocations(0)
			, myCurrentMemoryAllocated(0) {}

		unsigned int myAccumulatedMemoryAllocated;
		unsigned int myAccumulatedNumberOfAllocations;
		unsigned int myCurrentMemoryAllocated;
	};
}