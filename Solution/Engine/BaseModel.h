#pragma once
#include "EffectListener.h"
#include "VertexBufferWrapper.h"
#include <GrowingArray.h>

struct ID3D11BlendState;
struct ID3D11InputLayout;
struct ID3D11ShaderResourceView;
struct D3D11_BUFFER_DESC;
struct D3D11_INPUT_ELEMENT_DESC;
struct D3D11_SUBRESOURCE_DATA;

namespace Prism
{
	class Effect;
	class Surface;
	class Camera;
	struct IndexBufferWrapper;
	struct VertexBufferWrapper;

	class BaseModel : public EffectListener
	{
	public:
		BaseModel();
		virtual ~BaseModel();
		Effect* GetEffect();

		virtual const std::string& GetTechniqueName() const;

		virtual void SetFileName(const std::string& aFileName);

	protected:
		void EvaluateEffectTechnique(bool aInstanced);
		virtual void Render();

		void InitInputLayout(D3D11_INPUT_ELEMENT_DESC* aVertexDescArray, int aArraySize, const std::string& aDebugName);
		void InitVertexBuffer(int aVertexSize, int aBufferUsage, int aCPUUsage);
		void InitIndexBuffer();
		void InitSurface(const std::string& aResourceName, const std::string& aFileName);
		void InitSurface(const std::string& aResourceName, ID3D11ShaderResourceView* aShaderView);
		void InitBlendState(const std::string& aDebugName);

		void SetupVertexBuffer(int aVertexCount, int aVertexSize, char* aVertexData, const std::string& aDebugName);
		void SetupIndexBuffer(int aVertexCount, char* aIndexData, const std::string& aDebugName);

		virtual void OnEffectLoad();

		Effect* myEffect;
		CU::GrowingArray<Surface*> mySurfaces;
		CU::GrowingArray<D3D11_INPUT_ELEMENT_DESC*> myVertexFormat;
		ID3D11InputLayout* myVertexLayout;
		ID3D11BlendState* myBlendState;
		D3D11_BUFFER_DESC* myVertexBufferDesc;
		D3D11_BUFFER_DESC* myIndexBufferDesc;
		D3D11_SUBRESOURCE_DATA* myInitData;

		VertexBufferWrapper* myVertexBuffer;
		IndexBufferWrapper* myIndexBuffer;

		CU::Vector2<float> myPosition;
		CU::Vector2<float> myScale;

		std::string myFileName;
		std::string myTechniqueName;
	};

	inline const std::string& BaseModel::GetTechniqueName() const
	{
		return myTechniqueName;
	}

	inline void BaseModel::SetFileName(const std::string& aFileName)
	{
		myFileName = aFileName;
	}
}
