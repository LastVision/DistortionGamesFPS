#pragma once

#include "BoneName.h"
#include <GrowingArray.h>
#include <atomic>

struct ID3D11Texture2D;

#undef CreateFont

namespace Prism
{
	class AnimationProxy;
	class DGFXLoader;
	class FBXFactory;
	class FontProxy;
	class Model;
	class ModelProxy;
	class SpriteProxy;
	class Sprite;
	class TextProxy;

	class IModelFactory;

	class ModelLoader
	{
	public:
		static ModelLoader* GetInstance();
		static void Destroy();

		void Run();
		void Shutdown();
		void ClearLoadJobs();

		volatile bool IsLoading() const;
		volatile bool IsPaused() const;
		void Pause();
		void UnPause();
		void WaitUntilFinished() const;

		ModelProxy* LoadModel(const std::string& aModelPath, const std::string& aEffectPath);
		ModelProxy* LoadModelAnimated(const std::string& aModelPath, const std::string& aEffectPath);
		ModelProxy* LoadCube(float aWidth = 1.f, float aHeight = 1.f, float aDepth = 1.f
			, CU::Vector4f aColour = { 1.f, 1.f, 1.f, 1.f });

		AnimationProxy* LoadAnimation(const std::string& aPath);
		SpriteProxy* LoadSprite(const std::string& aPath, const CU::Vector2<float>& aSize
			, const CU::Vector2<float>& aHotSpot = { 0.f, 0.f });
		SpriteProxy* LoadSprite(ID3D11Texture2D* aD3D11Texture, const CU::Vector2<float>& aSize
			, const CU::Vector2<float>& aHotSpot = { 0.f, 0.f });

		FontProxy* LoadFont(const std::string& aFilePath, const CU::Vector2<int>& aTextureSize);
		TextProxy* LoadText(FontProxy* aFontProxy, bool aIs3d = false);

		void GetHierarchyToBone(const std::string& aAnimationPath, const std::string& aBoneName, GUIBone& aBoneOut);

	private:
		enum class eLoadType
		{
			MODEL,
			MODEL_ANIMATED,
			ANIMATION,
			CUBE,
			SPRITE,
			GUI_BONE,
			FONT,
			TEXT,
		};

		struct LoadData
		{
			union
			{
				ModelProxy* myModelProxy;
				AnimationProxy* myAnimationProxy;
				SpriteProxy* mySpriteProxy;
				GUIBone* myGUIBone;
				FontProxy* myFontProxy;
				TextProxy* myTextProxy;
			};

			eLoadType myLoadType;
			std::string myResourcePath = "";
			std::string myEffectPath = "";
			std::string myBoneName = "";
			CU::Vector4<float> mySize; //Cube uses X/Y/Z, Sprite uses X/Y as size and Z/W as hotspot
			CU::Vector4<float> myColor;
			ID3D11Texture2D* myD3D11Texture = nullptr;
			FontProxy* myFontProxyToUse;

			bool myIs3dText;
		};

		ModelLoader();
		~ModelLoader();

		void LoadInstancedCount();
		int GetInstancedCount(const std::string& aModelPath);

		bool CheckIfWorking();
		void WaitUntilCopyIsAllowed();
		void WaitUntilAddIsAllowed();
		void CopyLoadJobs();

		void CreateModel(LoadData& someData);
		void CreateModelAnimated(LoadData& someData);
		void CreateAnimation(LoadData& someData);
		void CreateCube(LoadData& someData);
		void CreateSprite(LoadData& someData);
		void CreateFont(LoadData& someData);
		void CreateText(LoadData& someData);
		void GetHierarchyToBone(LoadData& someData);

		CU::GrowingArray<LoadData> myBuffers[2];
		CU::GrowingArray<LoadData> myLoadArray;
		int myActiveBuffer;
		int myInactiveBuffer;
		volatile bool myCanAddToLoadArray;
		volatile bool myCanCopyArray;
		volatile bool myIsRunning;
		volatile bool myIsLoading;
		volatile bool myClearLoadJobs;
		volatile bool myIsPaused;

		IModelFactory* myModelFactory;
		CU::GrowingArray<Model*> myNonFXBModels;
		std::unordered_map<std::string, ModelProxy*> myModelProxies;
		std::unordered_map<std::string, Sprite*> mySprites;
		std::unordered_map<std::string, FontProxy*> myFontProxies;
		std::unordered_map<std::string, int> myInstancedCount;

		static ModelLoader* myInstance;
	};
}