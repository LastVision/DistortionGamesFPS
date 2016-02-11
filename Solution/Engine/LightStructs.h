#pragma once
#include <Vector.h>

namespace Prism
{
	struct DirectionalLightData
	{
		CU::Vector4f myDirection;
		CU::Vector4f myColor;
	};

	struct PointLightData
	{
		CU::Vector4f myPosition;
		CU::Vector4f myColor;
		float myRange;
	};

	struct SpotLightData
	{
		CU::Vector4f myPosition;
		CU::Vector4f myDirection;
		CU::Vector4f myColor;
		float myRange;
		float myCone;
	};
}