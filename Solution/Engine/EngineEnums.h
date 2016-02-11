#pragma once

namespace Prism
{
	enum class eOctreeType
	{
		NOT_IN_OCTREE,
		STATIC,
		DYNAMIC,
		PLAYER,
	};

	enum ePostProcessing
	{
		NONE = 0,
		BLOOM = 1,
		FOG_OF_WAR = 2,
	};

	enum class eFont
	{
		DIALOGUE,
		CONSOLE
	};
}