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

class ClientLevel;
class ClientLevelFactory;
class ClientGame;

namespace Prism
{
	class Camera;
	class DeferredRenderer;
	class Scene;
	class Instance;
}

namespace GUI
{
	class Cursor;
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
	ClientGame* myGame;

	ClientLevel* myLevel;
	ClientLevelFactory* myLevelFactory;
	GUI::Cursor* myCursor;

	Prism::Camera* myCamera;
	CU::Matrix44<float> myPlayerMatrix;
};

