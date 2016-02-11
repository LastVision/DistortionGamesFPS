#pragma once
#include "AIMap.h"

namespace Prism
{
	class Texture;
	class SpriteProxy;
}

class FogOfWarMap :	public AIMap
{
public:
	static FogOfWarMap* GetInstance();
	static void Destroy();

	void Update(float aDelta);
	void AddValue(float aValue, float aRadius, const CU::Vector2<float>& aPosition) override;

	void UpdateRenderPlane() override;

	bool IsVisible(const CU::Vector2<float>& aPosition);
	void ToggleFogOfWar();

	void SetFogOfWarHelperTexture(Prism::Texture* aTexture);

	void RenderSprite();

private:
	FogOfWarMap();
	~FogOfWarMap();

	bool myFogEnabled;
	float myLearnSpeed;

	Prism::Texture* myFogOfWarHelperTexture;
	Prism::SpriteProxy* mySprite;
	static FogOfWarMap* myInstance;
};

