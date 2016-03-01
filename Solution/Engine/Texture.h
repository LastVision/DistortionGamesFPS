#pragma once

#include <string>

struct ID3D11DepthStencilView;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;

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

		void ClearDepth();

		void Release();
		void Resize(float aWidth, float aHeight);

	private:
		void CreateDepthStencilView(float aWidth, float aHeight);

		std::string myFileName;
		ID3D11ShaderResourceView* myShaderView;
		ID3D11RenderTargetView* myRenderTargetView;
		int myTextureFormat;

		ID3D11DepthStencilView* myDepthStencilView;
		ID3D11ShaderResourceView* myDepthStencilShaderView;

		ID3D11Texture2D* myTexture;
		ID3D11Texture2D* myDepthTexture;
		bool myIsDepthTexture;

	};
}