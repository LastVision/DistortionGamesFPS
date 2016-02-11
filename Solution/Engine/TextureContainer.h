#pragma once
#include <unordered_map>
namespace Prism
{
	class Texture;

	class TextureContainer
	{
	public:
		static TextureContainer* GetInstance();
		static void Destroy();


		Texture* GetTexture(const std::string& aFileName);

	private:
		TextureContainer(){};
		~TextureContainer();
		void LoadTexture(const std::string& aFileName);
		void ReloadTexture(const std::string& aFileName);

		std::unordered_map<std::string, Texture*> myTextures;

		static TextureContainer* myInstance;
	};
}