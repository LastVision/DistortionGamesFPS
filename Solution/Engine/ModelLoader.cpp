#include "stdafx.h"

#include "AnimationProxy.h"
#include <CommonHelper.h>
#include "Engine.h"
#include "EffectContainer.h"
#include "Model.h"
#include "ModelAnimated.h"
#include "ModelLoader.h"
#include "ModelProxy.h"
#include "FBXFactory.h"
#include <TimerManager.h>
#include "SpriteProxy.h"
#include "Sprite.h"
#include <XMLReader.h>

#include "DGFXLoader.h"
#include <istream>

namespace Prism
{
	ModelLoader* ModelLoader::myInstance = nullptr;

	ModelLoader* ModelLoader::GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new ModelLoader();
		}

		return myInstance;
	}

	void ModelLoader::Destroy()
	{
		delete myInstance;
	}

	ModelLoader::ModelLoader()
		: myNonFXBModels(4)
		, myIsRunning(true)
		, myCanAddToLoadArray(true)
		, myCanCopyArray(true)
		, myModelFactory(nullptr)
		, myIsLoading(false)
		, myClearLoadJobs(true)
		, myIsPaused(false)
	{
		myBuffers[0].Init(4096);
		myBuffers[1].Init(4096);
		myLoadArray.Init(8192);
		myActiveBuffer = 0;
		myInactiveBuffer = 1;

#ifdef USE_DGFX
		myModelFactory = new DGFXLoader();
#else
		myModelFactory = new FBXFactory();
#endif

		LoadInstancedCount();
	}

	ModelLoader::~ModelLoader()
	{
		delete myModelFactory;
		myModelFactory = nullptr;
		myNonFXBModels.DeleteAll();

		for (auto it = myModelProxies.begin(); it != myModelProxies.end(); ++it)
		{
			SAFE_DELETE(it->second);
		}
		myModelProxies.clear();

		for (auto it = mySprites.begin(); it != mySprites.end(); ++it)
		{
			SAFE_DELETE(it->second);
		}
		mySprites.clear();
	}

	void ModelLoader::LoadInstancedCount()
	{
		std::ifstream file;
		file.open("GeneratedData/modelcount.bin");
		DL_ASSERT_EXP(file.is_open(), "Failed to open modelcount.bin, did you run Terrain-Tool?");
		if (file.is_open())
		{
			std::string name;
			int count;

			while (file >> name)
			{
				file >> count;
				myInstancedCount[name] = count;
			}
		}
	}

	int ModelLoader::GetInstancedCount(const std::string& aModelPath)
	{
		std::string name = aModelPath.substr(0, aModelPath.rfind("."));
		name = name.substr(aModelPath.rfind("/")+1, aModelPath.length());

		/*DL_ASSERT_EXP(myInstancedCount.find(name) != myInstancedCount.end()
			, CU::Concatenate("GetInstancedCount on %s failed", aModelPath.c_str()));*/
		//if (myInstancedCount.find(name) == myInstancedCount.end())
		{
			return 1024;
		}

		return myInstancedCount[name];
	}

	void ModelLoader::Run()
	{
#ifndef THREADED_LOADING
		return;
#else
		MEMORY_TRACKER_ALLOW_NEW_DURING_RUNTIME(std::this_thread::get_id());
		while (myIsRunning == true)
		{
			if (CheckIfWorking() == false)
			{
				continue;
			}

			WaitUntilCopyIsAllowed();
			myCanAddToLoadArray = false;

			CopyLoadJobs();

			for (int i = myLoadArray.Size() - 1; i >= 0; --i)
			{
				if (myIsPaused == true || myIsRunning == false)
				{
					break;
				}

				if (myClearLoadJobs == true)
				{
					myIsLoading = false;
					myClearLoadJobs = false;
					myLoadArray.RemoveAll();
					break;
				}

				eLoadType loadType = myLoadArray[i].myLoadType;

				switch (loadType)
				{
				case Prism::ModelLoader::eLoadType::MODEL:
				{
					CreateModel(myLoadArray[i]);
					break;
				}
				case Prism::ModelLoader::eLoadType::MODEL_ANIMATED:
				{
					CreateModelAnimated(myLoadArray[i]);
					break;
				}
				case Prism::ModelLoader::eLoadType::ANIMATION:
				{
					CreateAnimation(myLoadArray[i]);
					break;
				}
				case Prism::ModelLoader::eLoadType::CUBE:
				{
					CreateCube(myLoadArray[i]);
					break;
				}
				case Prism::ModelLoader::eLoadType::SPRITE:
				{
					CreateSprite(myLoadArray[i]);
					break;
				}
				default:
					DL_ASSERT("ModelLoader tried to load something that dint have a specified LoadType!!!");
					break;
				}

				myLoadArray.RemoveCyclicAtIndex(i);
			}
		}
#endif
	}

	void ModelLoader::Shutdown()
	{
		myIsRunning = false;
	}

	void ModelLoader::ClearLoadJobs()
	{
		myClearLoadJobs = true;
	}

	volatile bool ModelLoader::IsLoading() const
	{
		return myIsLoading;
	}

	volatile bool ModelLoader::IsPaused() const
	{
		return myIsPaused;
	}

	void ModelLoader::Pause()
	{
		DL_ASSERT_EXP(myIsPaused == false, "Can't pause when already paused.");
		myIsPaused = true;

		while (myIsLoading == true)
		{
		}
	}

	void ModelLoader::UnPause()
	{
		DL_ASSERT_EXP(myIsPaused == true, "Can't unpause when already unpaused.");
		myIsPaused = false;
		if (myBuffers[myInactiveBuffer].Size() == 0)
		{
			return;
		}

		while (myIsLoading == false)
		{
		}
	}

	void ModelLoader::WaitUntilFinished() const
	{
		while (myIsLoading == true)
		{
		}
	}


	ModelProxy* ModelLoader::LoadModel(const std::string& aModelPath, const std::string& aEffectPath)
	{
		if (myModelProxies.find(aModelPath) != myModelProxies.end())
		{
			return myModelProxies[aModelPath];
		}

		ModelProxy* proxy = new ModelProxy();
		proxy->SetModel(nullptr);

#ifdef THREADED_LOADING
		WaitUntilAddIsAllowed();

		myCanCopyArray = false;

		LoadData newData;
		newData.myModelProxy = proxy;
		newData.myLoadType = eLoadType::MODEL;
		newData.myModelPath = aModelPath;
		newData.myEffectPath = aEffectPath;

		myBuffers[myInactiveBuffer].Add(newData);
		myCanCopyArray = true;
#else
		Model* model = nullptr;
		model = myModelFactory->LoadModel(aModelPath);
		model->SetEffect(EffectContainer::GetInstance()->GetEffect(aEffectPath));
		model->myFileName = aModelPath;
		model->Init(GetInstancedCount(aModelPath));

		proxy->SetModel(model);
#endif
		
		myModelProxies[aModelPath] = proxy;
		return proxy;
	}

	ModelProxy* ModelLoader::LoadModelAnimated(const std::string& aModelPath, const std::string& aEffectPath)
	{
		if (myModelProxies.find(aModelPath) != myModelProxies.end())
		{
			return myModelProxies[aModelPath];
		}

		ModelProxy* proxy = new ModelProxy();
		proxy->SetModel(nullptr);

#ifdef THREADED_LOADING
		WaitUntilAddIsAllowed();

		myCanCopyArray = false;

		LoadData newData;
		newData.myModelProxy = proxy;
		newData.myLoadType = eLoadType::MODEL_ANIMATED;
		newData.myModelPath = aModelPath;
		newData.myEffectPath = aEffectPath;

		myBuffers[myInactiveBuffer].Add(newData);

		myCanCopyArray = true;
#else
		ModelAnimated* model = nullptr;
		model = myModelFactory->LoadAnimatedModel(aModelPath.c_str());
		model->SetEffect(EffectContainer::GetInstance()->GetEffect(aEffectPath));
		model->myFileName = aModelPath;
		model->Init();

		proxy->SetModelAnimated(model);
#endif

		myModelProxies[aModelPath] = proxy;
		return proxy;
	}

	ModelProxy* ModelLoader::LoadCube(float aWidth, float aHeight, float aDepth
		, CU::Vector4f aColour)
	{
#ifdef THREADED_LOADING
		WaitUntilAddIsAllowed();


		myCanCopyArray = false;
		ModelProxy* proxy = new ModelProxy();
		proxy->SetModel(nullptr);

		LoadData newData;
		newData.myModelProxy = proxy;
		newData.myLoadType = eLoadType::CUBE;
		newData.mySize = { aWidth, aHeight, aDepth, 0.f };
		newData.myColor = aColour;

		myBuffers[myInactiveBuffer].Add(newData);

		myCanCopyArray = true;

		return proxy;
#else
		ModelProxy* proxy = new ModelProxy();
		Model* model = new Prism::Model();
		model->InitCube(aWidth, aHeight, aDepth, aColour);

		proxy->SetModel(model);

		return proxy;
#endif	
	}

	AnimationProxy* ModelLoader::LoadAnimation(const std::string& aPath)
	{
		AnimationProxy* anim = new AnimationProxy();

#ifdef THREADED_LOADING
		WaitUntilAddIsAllowed();
		myCanCopyArray = false;

		LoadData animData;
		animData.myLoadType = eLoadType::ANIMATION;
		animData.myModelPath = aPath;
		animData.myAnimationProxy = anim;

		myBuffers[myInactiveBuffer].Add(animData);
		myCanCopyArray = true;
#else
		anim->myAnimation = myModelFactory->LoadAnimation(aPath);
#endif

		return anim;
	}

	SpriteProxy* ModelLoader::LoadSprite(const std::string& aPath, const CU::Vector2<float>& aSize
		, const CU::Vector2<float>& aHotSpot)
	{
		SpriteProxy* proxy = new SpriteProxy();
		proxy->mySprite = nullptr;
		proxy->SetSize(aSize, aHotSpot);

		if (mySprites.find(aPath) != mySprites.end())
		{
			proxy->mySprite = mySprites[aPath];
			return proxy;
		}
		

#ifdef THREADED_LOADING
		WaitUntilAddIsAllowed();
		myCanCopyArray = false;
		
		LoadData newData;
		newData.myModelPath = aPath;
		newData.mySpriteProxy = proxy;
		newData.myLoadType = eLoadType::SPRITE;
		newData.mySize = { aSize.x, aSize.y, aHotSpot.x, aHotSpot.y};

		myBuffers[myInactiveBuffer].Add(newData);
		myCanCopyArray = true;
#else
		mySprites[aPath] = new Sprite(aPath, aSize, aHotSpot);
		proxy->mySprite = mySprites[aPath];
#endif	

		return proxy;
	}

	SpriteProxy* ModelLoader::LoadSprite(ID3D11Texture2D* aD3D11Texture, const CU::Vector2<float>& aSize
		, const CU::Vector2<float>& aHotSpot)
	{
		SpriteProxy* proxy = new SpriteProxy();
		proxy->mySprite = nullptr;
		proxy->SetSize(aSize, aHotSpot);

#ifdef THREADED_LOADING
		WaitUntilAddIsAllowed();
		myCanCopyArray = false;

		LoadData newData;
		newData.myD3D11Texture = aD3D11Texture;
		newData.mySpriteProxy = proxy;
		newData.myLoadType = eLoadType::SPRITE;
		newData.mySize = { aSize.x, aSize.y, aHotSpot.x, aHotSpot.y };

		myBuffers[myInactiveBuffer].Add(newData);
		myCanCopyArray = true;
#else
		proxy->mySprite = new Sprite(aD3D11Texture, aSize, aHotSpot);
#endif	
		return proxy;
	}

	bool ModelLoader::CheckIfWorking()
	{
		if (myIsPaused == true || (myBuffers[myInactiveBuffer].Size() == 0
			&& myLoadArray.Size() == 0) || myClearLoadJobs == true)
		{
			myCanAddToLoadArray = true;
			myIsLoading = false;
			myClearLoadJobs = false;
			Engine::GetInstance()->myIsLoading = false;
			std::this_thread::yield();
			return false;
		}

		return true;
	}

	void ModelLoader::WaitUntilCopyIsAllowed()
	{
		while (myCanCopyArray == false)
			; //Should be an empty whileloop!
	}

	void ModelLoader::WaitUntilAddIsAllowed()
	{
		while (myCanAddToLoadArray == false)
			; //Should be an empty whileloop!
	}

	void ModelLoader::CopyLoadJobs()
	{
		int newInactive = myActiveBuffer;
		int newActive = myInactiveBuffer;

		myActiveBuffer = newActive;
		myInactiveBuffer = newInactive;

		myBuffers[myInactiveBuffer].RemoveAll();

		myCanAddToLoadArray = true;
		myIsLoading = true;
		Engine::GetInstance()->myIsLoading = true;

		for (int i = 0; i < myBuffers[myActiveBuffer].Size(); ++i)
		{
			myLoadArray.Add(myBuffers[myActiveBuffer][i]);
		}
	}

	void ModelLoader::CreateModel(LoadData& someData)
	{
		Model* model = myModelFactory->LoadModel(someData.myModelPath);
		model->SetEffect(EffectContainer::GetInstance()->GetEffect(someData.myEffectPath));
		model->myFileName = someData.myModelPath;
		model->Init(GetInstancedCount(someData.myModelPath));

		someData.myModelProxy->SetModel(model);
	}

	void ModelLoader::CreateModelAnimated(LoadData& someData)
	{
		ModelAnimated* model = myModelFactory->LoadAnimatedModel(someData.myModelPath);
		model->SetEffect(EffectContainer::GetInstance()->GetEffect(someData.myEffectPath));
		model->myFileName = someData.myModelPath;
		model->Init();

		someData.myModelProxy->SetModelAnimated(model);
	}

	void ModelLoader::CreateAnimation(LoadData& someData)
	{
		someData.myAnimationProxy->myAnimation = myModelFactory->LoadAnimation(someData.myModelPath);
	}

	void ModelLoader::CreateCube(LoadData& someData)
	{
		Model* model = new Prism::Model();
		model->InitCube(someData.mySize.x, someData.mySize.y,
			someData.mySize.z, someData.myColor);

		someData.myModelProxy->SetModel(model);
		myNonFXBModels.Add(model);
	}

	void ModelLoader::CreateSprite(LoadData& someData)
	{
		if (someData.myD3D11Texture != nullptr)
		{
			CU::Vector2<float> size(someData.mySize.x, someData.mySize.y);
			CU::Vector2<float> hotSpot(someData.mySize.z, someData.mySize.w);

			someData.mySpriteProxy->mySprite = new Sprite(someData.myD3D11Texture, size, hotSpot);
		}
		else if (mySprites.find(someData.myModelPath) != mySprites.end())
		{
			someData.mySpriteProxy->mySprite = mySprites[someData.myModelPath];
		}
		else
		{
			CU::Vector2<float> size(someData.mySize.x, someData.mySize.y);
			CU::Vector2<float> hotSpot(someData.mySize.z, someData.mySize.w);

			mySprites[someData.myModelPath] = new Sprite(someData.myModelPath, size, hotSpot);

			someData.mySpriteProxy->mySprite = mySprites[someData.myModelPath];
		}
	}
}