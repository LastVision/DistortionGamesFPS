#pragma once
#include "Component.h"

namespace Prism
{
	class Instance;
	struct MeshData;
	class Camera;
	class Texture;
};

struct GraphicsComponentData;

class GraphicsComponent : public Component
{
public:
	GraphicsComponent(Entity& aEntity, const GraphicsComponentData& aComponentData);
	~GraphicsComponent();

	void InitDLL(const char* aModelPath, const char* aEffectPath);
	void InitCube(float aWidth, float aHeight, float aDepth);
	void Update(float aDeltaTime);
	Prism::Instance* GetInstance();
	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void SetPosition(const CU::Vector3<float>& aPosition);
	void SetRotation(const CU::Vector3<float>& aRotation);
	void SetScale(const CU::Vector3<float>& aScale);

private:
	Prism::Instance* myInstance;
	const GraphicsComponentData& myComponentData;
};

inline Prism::Instance* GraphicsComponent::GetInstance()
{
	return myInstance;
}

inline eComponentType GraphicsComponent::GetTypeStatic()
{
	return eComponentType::GRAPHICS;
}

inline eComponentType GraphicsComponent::GetType()
{
	return GetTypeStatic();
}