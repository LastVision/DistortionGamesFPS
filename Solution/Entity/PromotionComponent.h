#pragma once
#include "Component.h"

namespace Prism
{
	class SpriteProxy;
}

struct PromotionComponentData;
class PromotionComponent :
	public Component
{
public:
	PromotionComponent(Entity& aEntity, PromotionComponentData& aData);
	~PromotionComponent();
	void Reset() override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void RenderPromotion(const Prism::Camera& aCamera);
	void RenderPromotion(const CU::Vector2<float>& aPosition, const CU::Vector2<float>& aScale = { 1.f, 1.f }
		, const CU::Vector4<float>& aColor = { 1.f, 1.f, 1.f, 1.f });

	void EnemyKilled();
	bool GetPromoted() const;


private:
	void operator=(PromotionComponent&) = delete;
	void Promote();
	bool myIsPromoted;
	Prism::SpriteProxy* mySprite;

	int myKillCount;

	int myCurrentKillCount;
};

inline eComponentType PromotionComponent::GetTypeStatic()
{
	return eComponentType::PROMOTION;
}

inline eComponentType PromotionComponent::GetType()
{
	return GetTypeStatic();
}

inline void PromotionComponent::EnemyKilled()
{
	++myCurrentKillCount;

	if (myIsPromoted == false)
	{
		Promote();
	}
}

inline bool PromotionComponent::GetPromoted() const
{
	return myIsPromoted;
}