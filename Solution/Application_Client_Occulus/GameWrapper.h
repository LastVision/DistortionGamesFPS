#pragma once

#include <DirectXMath.h>
#include <Vector.h>
#include <Matrix.h>

#include <Model.h>
#include <GrowingArray.h>

struct ID3D11DeviceContext;
struct ID3D11Device;

class GameWrapper
{
public:
	GameWrapper();
	~GameWrapper();

	void SetContext(ID3D11DeviceContext* aContext);
	void SetDevice(ID3D11Device* aDevice);

	void Init();

	void Render(const DirectX::XMMATRIX& aViewProjection);
	CU::Matrix44<float> ConvertMatrix(const DirectX::XMMATRIX& aMatrix);

private:
	CU::GrowingArray<CU::Matrix44<float>> myOrientations;
	CU::GrowingArray<Prism::Model*> myModels;
};

