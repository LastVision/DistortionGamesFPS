#pragma once
#include <Vector.h>
namespace Prism
{
	enum class VertexType
	{
		POS_COLOR,
		POS_UV,
		POS_COLOR_UV,
		POS_NORM_UV,
		POS_NORM_COLOR,
	};

	struct VertexPosColor
	{
		CU::Vector4<float> myPos;
		CU::Vector4<float> myCol;
	};

	struct VertexPosUV
	{
		CU::Vector3<float> myPos;
		CU::Vector2<float> myUV;
	};

	struct VertexPosColorUV
	{
		CU::Vector3<float> myPos;
		CU::Vector4<float> myCol;
		CU::Vector2<float> myUV;
	};

	struct VertexPosNormUV
	{
		CU::Vector3<float> myPos;
		CU::Vector3<float> myNorm;
		CU::Vector2<float> myUV;
	};

	struct VertexPosNormUVBiTang
	{
		CU::Vector3<float> myPosition;
		CU::Vector3<float> myNormal;
		CU::Vector2<float> myUV;
		CU::Vector3<float> myBiNormal;
		CU::Vector3<float> myTangent;
	};

	struct GUIVertex
	{
		CU::Vector4<float> myPosition;
		CU::Vector2<float> myUV;
	};

	struct VertexPosNormColor
	{
		CU::Vector4<float> myPosition;
		CU::Vector4<float> myNormal;
		CU::Vector4<float> myColor;
	};
}