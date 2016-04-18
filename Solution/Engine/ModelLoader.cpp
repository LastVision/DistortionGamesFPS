#include "stdafx.h"

#include "AnimationProxy.h"
#include <CommonHelper.h>
#include "Engine.h"
#include "EffectContainer.h"
#include "Font.h"
#include "FontProxy.h"
#include "Model.h"
#include "ModelAnimated.h"
#include "ModelLoader.h"
#include "ModelProxy.h"
#include "FBXFactory.h"
#include "Text.h"
#include "TextProxy.h"
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
		LoadRadiuses();
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

		for (auto it = myFontProxies.begin(); it != myFontProxies.end(); ++it)
		{
			SAFE_DELETE(it->second);
		}
		myFontProxies.clear();
	}

	void ModelLoader::LoadInstancedCount()
	{
		std::ifstream file;
		file.open("GeneratedData/modelcount.bin");
		DL_ASSERT_EXP(file.is_open(), "Failed to open modelcount.bin, did you run the DGFX-Tool?");
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
		if (myInstancedCount.find(name) == myInstancedCount.end())
		{
			return 2048;
		}

		return int(myInstancedCount[name] * 1.5f);
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

			DEBUG_PRINT("MODELLOADER IS WORKING");

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
				case eLoadType::MODEL:
				{
					CreateModel(myLoadArray[i]);
					break;
				}
				case eLoadType::MODEL_ANIMATED:
				{
					CreateModelAnimated(myLoadArray[i]);
					break;
				}
				case eLoadType::ANIMATION:
				{
					CreateAnimation(myLoadArray[i]);
					break;
				}
				case eLoadType::CUBE:
				{
					CreateCube(myLoadArray[i]);
					break;
				}
				case eLoadType::SPRITE:
				{
					CreateSprite(myLoadArray[i]);
					break;
				}
				case eLoadType::GUI_BONE:
				{
					GetHierarchyToBone(myLoadArray[i]);
					break;
				}
				case eLoadType::FONT:
				{
					CreateFont(myLoadArray[i]);
					break;
				}
				case eLoadType::TEXT:
				{
					CreateText(myLoadArray[i]);
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
		//DL_ASSERT_EXP(myIsPaused == false, "Can't pause when already paused.");
		myIsPaused = true;

		while (myIsLoading == true)
		{
		}
	}

	void ModelLoader::UnPause()
	{
		//DL_ASSERT_EXP(myIsPaused == true, "Can't unpause when already unpaused.");
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
		newData.myResourcePath = aModelPath;
		newData.myEffectPath = aEffectPath;

		myBuffers[myInactiveBuffer].Add(newData);
		myCanCopyArray = true;
#else
		bool lightMesh = aEffectPath.find("light_mesh") != std::string::npos;
		Model* model = nullptr;
		model = myModelFactory->LoadModel(aModelPath);
		model->SetEffect(EffectContainer::GetInstance()->GetEffect(aEffectPath));
		model->myFileName = aModelPath;

		
		model->Init(GetInstancedCount(aModelPath), lightMesh);

		proxy->SetModel(model);
#endif
		
		myModelProxies[aModelPath] = proxy;
		
		SetRadius(proxy, aModelPath);

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
		newData.myResourcePath = aModelPath;
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

		SetRadius(proxy, aModelPath);

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
		animData.myResourcePath = aPath;
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
		newData.myResourcePath = aPath;
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

	FontProxy* ModelLoader::LoadFont(const std::string& aFilePath, const CU::Vector2<int>& aTextureSize)
	{
		if (myFontProxies.find(aFilePath) != myFontProxies.end())
		{
			return myFontProxies[aFilePath];
		}

		FontProxy* proxy = new FontProxy(aFilePath);

#ifdef THREADED_LOADING
		WaitUntilAddIsAllowed();
		myCanCopyArray = false;

		LoadData newData;
		newData.myResourcePath = aFilePath;
		newData.myFontProxy = proxy;
		newData.mySize = CU::Vector4<float>(float(aTextureSize.x), float(aTextureSize.y), 0.f, 0.f);
		newData.myLoadType = eLoadType::FONT;

		myBuffers[myInactiveBuffer].Add(newData);
		myCanCopyArray = true;
#else
		proxy->SetFont(new Font(aFilePath, aTextureSize));
		myFontProxies[aFilePath] = proxy;
#endif	

		return proxy;
	}

	TextProxy* ModelLoader::LoadText(FontProxy* aFontProxy, bool aIs3d, bool aShouldFollowCamera)
	{
		TextProxy* proxy = new TextProxy();

#ifdef THREADED_LOADING
		WaitUntilAddIsAllowed();
		myCanCopyArray = false;

		LoadData newData;
		newData.myTextProxy = proxy;
		newData.myFontProxyToUse = aFontProxy;
		newData.myLoadType = eLoadType::TEXT;
		newData.myIs3dText = aIs3d;
		newData.myShouldFollowCamera = aShouldFollowCamera;

		myBuffers[myInactiveBuffer].Add(newData);
		myCanCopyArray = true;
#else
		proxy->SetText(new Text(*aFontProxy, aIs3d, aShouldFollowCamera));
#endif	

		return proxy;
	}

	void ModelLoader::GetHierarchyToBone(const std::string& aAnimationPath, const std::string& aBoneName, GUIBone& aBoneOut)
	{
#ifdef THREADED_LOADING
		WaitUntilAddIsAllowed();
		myCanCopyArray = false;

		LoadData newData;
		newData.myResourcePath = aAnimationPath;
		newData.myBoneName = aBoneName;
		newData.myGUIBone = &aBoneOut;
		newData.myLoadType = eLoadType::GUI_BONE;

		myBuffers[myInactiveBuffer].Add(newData);
		myCanCopyArray = true;
#else
		myModelFactory->GetHierarchyToBone(aAnimationPath, aBoneName, aBoneOut);
		aBoneOut.myIsValid = true;
#endif	
	}

	void ModelLoader::SetRadius(ModelProxy* aProxy, const std::string& aModelPath)
	{
		float radius = 0;
		if (myRadiuses.find(aModelPath) == myRadiuses.end())
		{
			radius = 25.f;
		}
		else
		{
			radius = myRadiuses[aModelPath];
		}

		aProxy->SetRadius(radius);
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

	void ModelLoader::LoadRadiuses()
	{
		std::ifstream file;
		file.open("GeneratedData/modellist.bin", std::ios::in | std::ios::binary);

		std::string model;
		float radius = 0;
		while (file >> model)
		{
			file >> radius;

			myRadiuses[model] = radius;
		}
	}

	void ModelLoader::CreateModel(LoadData& someData)
	{
		Model* model = myModelFactory->LoadModel(someData.myResourcePath);
		model->SetEffect(EffectContainer::GetInstance()->GetEffect(someData.myEffectPath));
		model->myFileName = someData.myResourcePath;

		bool lightMesh = someData.myEffectPath.find("light_mesh") != std::string::npos;
		model->Init(GetInstancedCount(someData.myResourcePath), lightMesh);

		someData.myModelProxy->SetModel(model);
	}

	void ModelLoader::CreateModelAnimated(LoadData& someData)
	{
		ModelAnimated* model = myModelFactory->LoadAnimatedModel(someData.myResourcePath);
		model->SetEffect(EffectContainer::GetInstance()->GetEffect(someData.myEffectPath));
		model->myFileName = someData.myResourcePath;
		model->Init();

		someData.myModelProxy->SetModelAnimated(model);
	}

	void ModelLoader::CreateAnimation(LoadData& someData)
	{
		someData.myAnimationProxy->myAnimation = myModelFactory->LoadAnimation(someData.myResourcePath);
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
		else if (mySprites.find(someData.myResourcePath) != mySprites.end())
		{
			someData.mySpriteProxy->mySprite = mySprites[someData.myResourcePath];
		}
		else
		{
			CU::Vector2<float> size(someData.mySize.x, someData.mySize.y);
			CU::Vector2<float> hotSpot(someData.mySize.z, someData.mySize.w);

			mySprites[someData.myResourcePath] = new Sprite(someData.myResourcePath, size, hotSpot);

			someData.mySpriteProxy->mySprite = mySprites[someData.myResourcePath];
		}
	}

	void ModelLoader::CreateFont(LoadData& someData)
	{
		CU::Vector2<int> size(int(someData.mySize.x), int(someData.mySize.y));
		someData.myFontProxy->SetFont(new Font(someData.myResourcePath, size));
		myFontProxies[someData.myFontProxy->GetFilePath()] = someData.myFontProxy;
	}

	void ModelLoader::CreateText(LoadData& someData)
	{
		someData.myTextProxy->SetText(new Text(*someData.myFontProxyToUse, someData.myIs3dText, someData.myShouldFollowCamera));
	}

	void ModelLoader::GetHierarchyToBone(LoadData& someData)
	{
		myModelFactory->GetHierarchyToBone(someData.myResourcePath, someData.myBoneName, *someData.myGUIBone);
		someData.myGUIBone->myIsValid = true;
	}
}