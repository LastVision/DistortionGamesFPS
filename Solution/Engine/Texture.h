#pragma once

#include <string>

struct D3D11_VIEWPORT;
struct ID3D11DepthStencilView;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;
struct ID3D11Texture3D;

namespace Prism
{
	class Texture
	{
	public:
		Texture();
		~Texture();

		void Init(float aWidth, float aHeight, unsigned int aBindFlag
			, unsigned int aFormat);

		void InitAsDepthBuffer(float aWidth, float aHeight);
		void CopyDepthBuffer(ID3D11Texture2D* aSource);

		bool LoadTexture(const std::string& aFilePath);
		const std::string& GetFileName() const;

		ID3D11ShaderResourceView* GetShaderView() const;
		ID3D11RenderTargetView* GetRenderTargetView() const;
		ID3D11Texture2D* GetTexture() const;

		ID3D11ShaderResourceView* GetDepthStencilShaderView() const;
		ID3D11DepthStencilView* GetDepthStencilView() const;
		ID3D11Texture2D* GetDepthTexture() const;

		ID3D11RenderTargetView* GetCubemapRenderTarget(int aIndex);
		D3D11_VIEWPORT* GetViewPort();

		void CreateDepthStencilView(float aWidth, float aHeight, int aArraySize = 1);
		void CreateCubemap(float aWidth, float aHeight);
		void Create3DTexture(float* aData, int aWidth, int aHeight, int aDepth);

		void ClearDepth();

		void Release();
		void Resize(float aWidth, float aHeight);

		void SaveToFile(const std::string& aFilePath);
		void Save3DToFile(const std::string& aFilePath);

	private:

		std::string myFileName;
		ID3D11ShaderResourceView* myShaderView;
		ID3D11RenderTargetView* myRenderTargetView;
		int myTextureFormat;

		ID3D11DepthStencilView* myDepthStencilView;
		ID3D11ShaderResourceView* myDepthStencilShaderView;

		ID3D11Texture2D* myTexture;
		ID3D11Texture2D* myDepthTexture;
		bool myIsDepthTexture;

		ID3D11Texture3D* myD3D11Texture3D;

		ID3D11RenderTargetView* myCubemapRenderTargets[6];
		D3D11_VIEWPORT* myViewPort;
	};
}