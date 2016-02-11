#pragma once

namespace Prism
{
	class Instance;
}

struct ShadowBlobComponentData;

class ShadowBlobComponent : public Component
{
public:
	ShadowBlobComponent(Entity& aEntity, ShadowBlobComponentData& aComponentData);
	~ShadowBlobComponent();

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	Prism::Instance* GetShadowBlob();

private:
	Prism::Instance* myShadowBlob;
	float myCullingRadius;
};

inline eComponentType ShadowBlobComponent::GetTypeStatic()
{
	return eComponentType::SHADOWBLOB;
}

inline eComponentType ShadowBlobComponent::GetType()
{
	return GetTypeStatic();
}

inline Prism::Instance* ShadowBlobComponent::GetShadowBlob()
{
	return myShadowBlob;
}
