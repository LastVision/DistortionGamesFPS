#include "stdafx.h"
#include "SpriteAnimator.h"
#include "SpriteProxy.h"
#include <XMLReader.h>

namespace Prism
{
	SpriteAnimator::SpriteAnimator(std::string aXMLPath)
		: myTimeSinceLastFrame(0.f)
		, myCurrentFrame(0)
		, myCurrentAnimation(nullptr)
		, emptySprite(nullptr)
	{
		myAnimations.Init(1);

		XMLReader reader;
		reader.OpenDocument(aXMLPath);

		CU::Vector2<float> spriteHotspot(0.f, 0.f);
		CU::Vector2<float> frameSize;
		std::string spritePath = "";
		CU::Vector2<float> spriteSize(0.f, 0.f);
		float fps = 0.f;
		bool isLooping = true;
		int numberOfFrames = -1;

		tinyxml2::XMLElement* rootElement = reader.FindFirstChild("root");

		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "spritesheet"), "path", spritePath);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "spritesheet"), "sizex", spriteSize.x);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "spritesheet"), "sizey", spriteSize.y);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "fps"), "value", fps);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "islooping"), "value", isLooping);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "numberofframes"), "value", numberOfFrames);	
		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "framesize"), "x", frameSize.x);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "framesize"), "y", frameSize.y);
		reader.ReadAttribute(reader.FindFirstChild(rootElement, "framesize"), "hotspotx", spriteHotspot.x);
		reader.ReadAttribute(reader.FindFirstChild(rootElement, "framesize"), "hotspoty", spriteHotspot.y);

		AddAnimation(fps, numberOfFrames, frameSize, ModelLoader::GetInstance()->LoadSprite(spritePath, spriteSize, spriteHotspot), isLooping);

		reader.CloseDocument();
	}

	SpriteAnimator::~SpriteAnimator()
	{
		myAnimations.DeleteAll();
	}

	void SpriteAnimator::Update(float aDelta)
	{
		if (myCurrentAnimation == nullptr)
		{
			return;
		}
		myTimeSinceLastFrame += aDelta;

		if (myTimeSinceLastFrame >= myCurrentAnimation->GetTimePerFrame())
		{
			myTimeSinceLastFrame -= myCurrentAnimation->GetTimePerFrame();

			myCurrentFrame++;

			if (myCurrentFrame >= myCurrentAnimation->GetNumberOfFrames())
			{
				myCurrentFrame = 0;

				if (myCurrentAnimation->IsLooping() == false)
				{
					myCurrentAnimation = nullptr;
				}
			}
		}
	}

	void SpriteAnimator::Render(CU::Vector2<float> aPosition)
	{
		if (myCurrentAnimation != nullptr)
		{
			GetSpriteFrame()->Render(aPosition);
		}
	}

	void SpriteAnimator::StartAnimation(int anID)
	{
		DL_ASSERT_EXP(anID <= myAnimations.Size() - 1, "[SpriteAnimator] Trying to start animation at nonexisting index");
		myCurrentAnimation = myAnimations[anID];
	}

	void SpriteAnimator::RestartAnimation(int anID)
	{
		DL_ASSERT_EXP(anID <= myAnimations.Size() - 1, "[SpriteAnimator] Trying to start animation at nonexisting index");
		myCurrentFrame = 0;
		myCurrentAnimation = myAnimations[anID];
	}

	void SpriteAnimator::StopAnimation()
	{
		if (myCurrentAnimation != nullptr)
		{
			myCurrentAnimation = nullptr;
		}
	}

	void SpriteAnimator::ResetAnimation()
	{
		myCurrentFrame = 0;
	}

	void SpriteAnimator::AddAnimation(float aFPS, int aNumberOfFrames, CU::Vector2<float> aFrameSize
		, SpriteProxy* aSpriteSheet, bool aIsLooping)
	{
		myAnimations.Add(new SpriteAnimation(aFPS, aNumberOfFrames, aFrameSize, aSpriteSheet, aIsLooping));
	}

	SpriteProxy* SpriteAnimator::GetSpriteFrame()
	{
		if (myCurrentAnimation == nullptr)
		{
			return emptySprite;
		}

		int x = myCurrentFrame;
		int y = 0;

		for (; x >= myCurrentAnimation->GetFramesPerRow(); x -= myCurrentAnimation->GetFramesPerRow())
		{
			y++;
		}

		CU::Vector2<float> animationPosition(myCurrentAnimation->GetFrameSize().x * float(x), myCurrentAnimation->GetFrameSize().y * float(y));

		CU::Vector2<float> topLeft = animationPosition;
		CU::Vector2<float> rightBottom = (animationPosition + myCurrentAnimation->GetFrameSize());

		topLeft /= myCurrentAnimation->GetSpriteSheetSize();
		rightBottom /= myCurrentAnimation->GetSpriteSheetSize();

		myCurrentAnimation->GetSpriteSheet()->SetUVZeroToOne(topLeft, rightBottom);
		myCurrentAnimation->GetSpriteSheet()->SetSize(myCurrentAnimation->GetFrameSize(), myCurrentAnimation->GetSpriteSheet()->GetHotspot());
		return myCurrentAnimation->GetSpriteSheet();
	}

	bool SpriteAnimator::IsPlayingAnimation() const
	{
		return myCurrentAnimation != nullptr;
	}

	int SpriteAnimator::GetCurrentFrame()
	{
		return myCurrentFrame;
	}
}