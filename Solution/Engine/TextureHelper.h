#pragma once
#include "stdafx.h"
#include <CommonHelper.h>
#include <D3DX11async.h>

namespace Prism
{
	namespace TextureHelper
	{

		static HRESULT CreateShaderResourceViewFromFile(const std::string& aFilePath, ID3D11ShaderResourceView** aResourceToBind)
		{
			HRESULT hr = D3DX11CreateShaderResourceViewFromFile(Engine::GetInstance()->GetDevice(), aFilePath.c_str()
				, NULL, NULL, aResourceToBind, NULL);

			if (FAILED(hr) == S_OK)
			{
				ID3D11Resource* resource = nullptr;
				(*aResourceToBind)->GetResource(&resource);
				ID3D11Texture2D* texture2D = reinterpret_cast<ID3D11Texture2D*>(resource);
				D3D11_TEXTURE2D_DESC* texture2DDEsc = new D3D11_TEXTURE2D_DESC;
				texture2D->GetDesc(texture2DDEsc);
				UINT width = texture2DDEsc->Width;
				UINT height = texture2DDEsc->Height;
				texture2D->Release();
				delete texture2DDEsc;

				std::string errorMessage = "Texturesize not power of 2: [" + aFilePath + "].";
				DL_ASSERT_EXP(CU::IsValidTextureSize(height) && CU::IsValidTextureSize(width), errorMessage.c_str());

				if (aFilePath.empty() == false)
				{
					const char* fileEnding = &aFilePath[aFilePath.size() - 3];
					std::string stringEnding(fileEnding);
					CU::ToLower(stringEnding);
					std::string errorMessage = "Texture file-format not .DDS/.dds in [" + aFilePath + "].";
					DL_ASSERT_EXP(stringEnding == "dds", errorMessage.c_str());
				}
			}
			return hr;
		}
	}
}