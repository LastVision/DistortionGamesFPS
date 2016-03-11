#pragma once

namespace Prism
{
	struct CharacterData
	{
		CharacterData()
			: myAdvanceX(0.f)
		{}

		CU::Vector2<float> myTopLeftUV;
		CU::Vector2<float> myBottomRightUV;
		CU::Vector2<float> mySize;
		CU::Vector2<float> myOffset;
		float myAdvanceX;
	};
}