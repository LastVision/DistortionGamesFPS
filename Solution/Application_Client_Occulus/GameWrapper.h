#pragma once

#include <DirectXMath.h>
#include <Vector.h>
#include <Matrix.h>

#include <Model.h>
#include <GrowingArray.h>

struct ID3D11DeviceContext;
struct ID3D11Device;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;

namespace Prism
{
	class Camera;
	class DeferredRenderer;
	class Scene;
	class Instance;
}

class GameWrapper
{
public:
	GameWrapper(float aHeight, float aWidth, ID3D11Device* aDevice, ID3D11DeviceContext* aContext);
	~GameWrapper();

	void SetContext(ID3D11DeviceContext* aContext);
	void SetDevice(ID3D11Device* aDevice);
	void SetWindowSize(const CU::Vector2<float>& aWindowSize);

	void Init();

	void Update(float aDelta, const CU::Matrix44<float>& aView, const CU::Matrix44<float>& aProjection, const CU::Matrix44<float>& aViewProjection);

	void Render(const DirectX::XMMATRIX& aViewProjection, ID3D11RenderTargetView* aRenderTarget, ID3D11DepthStencilView* aDepthStencil);
	CU::Matrix44<float> ConvertMatrix(const DirectX::XMMATRIX& aMatrix);

private:
	CU::GrowingArray<CU::Matrix44<float>> myOrientations;
	CU::GrowingArray<Prism::Model*> myModels;

	Prism::Scene* myScene;
	Prism::DeferredRenderer* myRenderer;
	Prism::Camera* myCamera;
	CU::Matrix44<float> myTestOrientation;
	CU::Matrix44<float> myPlayerMatrix;

	Prism::Instance* myAnimation;
};

