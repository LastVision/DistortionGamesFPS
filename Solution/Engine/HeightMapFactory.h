#pragma once

namespace Prism
{
	struct HeightMap;

	class HeightMapFactory
	{
	public:
		static HeightMap* Create(const char* aFilePath);
	};
}