#pragma once

#include <GrowingArray.h>
#include <atomic>

struct ID3D11Texture2D;

namespace Prism
{
	class AnimationProxy;
	class Model;
	class ModelProxy;
	class FBXFactory;
	class DGFXLoader;
	class SpriteProxy;
	class Sprite;

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

	private:
		enum class eLoadType
		{
			MODEL,
			MODEL_ANIMATED,
			ANIMATION,
			CUBE,
			SPRITE,
		};

		struct LoadData
		{
			union
			{
				ModelProxy* myModelProxy;
				AnimationProxy* myAnimationProxy;
				SpriteProxy* mySpriteProxy;
			};

			eLoadType myLoadType;
			std::string myModelPath = "";
			std::string myEffectPath = "";
			CU::Vector4<float> mySize; //Cube uses X/Y/Z, Sprite uses X/Y as size and Z/W as hotspot
			CU::Vector4<float> myColor;
			ID3D11Texture2D* myD3D11Texture = nullptr;
		};

		ModelLoader();
		~ModelLoader();

		void StartPrefetching();
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

		FBXFactory* myModelFactory;
		DGFXLoader* myDGFXLoader;
		CU::GrowingArray<Model*> myNonFXBModels;
		std::unordered_map<std::string, ModelProxy*> myModelProxies;
		std::unordered_map<std::string, Sprite*> mySprites;
		std::unordered_map<std::string, int> myInstancedCount;

		static ModelLoader* myInstance;
	};
}