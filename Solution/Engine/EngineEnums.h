#pragma once

namespace Prism
{
	enum class eDepthStencil
	{
		Z_ENABLED,
		Z_DISABLED,
		READ_NO_WRITE,
		_COUNT,
	};

	enum class eRasterizer
	{
		CULL_BACK,
		WIRE_FRAME,
		NO_CULLING,
		_COUNT,
	};

	enum class eBlendState
	{
		ALPHA,
		NO_ALPHA,
		_COUNT,
	};

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