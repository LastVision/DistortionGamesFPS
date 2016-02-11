#include "stdafx.h"

#include <ActorComponent.h>
#include "FogOfWarMap.h"
#include <PollingStation.h>
#include <Entity.h>
#include <RenderPlane.h>
#include <TriggerComponent.h>
#include <SpriteProxy.h>
#include <ModelLoader.h>
#include <Texture.h>

#define OUTER_RING 10.f
#define MIN_DARKNESS 0.4f

FogOfWarMap* FogOfWarMap::myInstance = nullptr;
FogOfWarMap* FogOfWarMap::GetInstance()
{
	if (myInstance == nullptr)
	{
		myInstance = new FogOfWarMap();
	}

	return myInstance;
}

void FogOfWarMap::Destroy()
{
	SAFE_DELETE(myInstance);
	
}

FogOfWarMap::FogOfWarMap()
	: AIMap(256)
	, myFogEnabled(true)
	, mySprite(nullptr)
	, myLearnSpeed(1.5f)
{
	myMaxValue = 1.f;
}

FogOfWarMap::~FogOfWarMap()
{
	SAFE_DELETE(mySprite);
}

void FogOfWarMap::Update(float aDelta)
{
	for (int i = 0; i < myGrid.Size(); ++i)
	{
		//myGrid[i] = MIN_DARKNESS;
		//myGrid[i] -= 0.5f * aDelta;
		//myGrid[i] -= (myGrid[i] * 0.5f) * aDelta;
		myGrid[i] *= 0.99f;
		myGrid[i] = fmaxf(myGrid[i], MIN_DARKNESS);
		myPlane->SetVertexColor(i, GetColor(myGrid[i] / myMaxValue));
	}

	if (PollingStation::GetInstance()->GetBase(eOwnerType::PLAYER)->IsSelectable() == true)
	{
		AddValue(myLearnSpeed*aDelta, 20.f, PollingStation::GetInstance()->GetBase(eOwnerType::PLAYER)->GetPosition());
	}

	const CU::GrowingArray<Entity*>& playerUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER);
	for (int j = 0; j < playerUnits.Size(); ++j)
	{
		float radius = playerUnits[j]->GetComponent<ActorComponent>()->GetVisionRange();
		AddValue(myLearnSpeed*aDelta, radius, playerUnits[j]->GetPosition());
	}

	const CU::GrowingArray<Entity*>& playerPoints = PollingStation::GetInstance()->GetVictoryAndResourcePoints();
	for (int i = 0; i < playerPoints.Size(); ++i)
	{
		if (playerPoints[i]->GetComponent<TriggerComponent>()->GetOwnerGainingPoint() == eOwnerType::PLAYER)
		{
			AddValue(myLearnSpeed*aDelta, 20.f, playerPoints[i]->GetPosition());
		}
	}
}

void FogOfWarMap::AddValue(float aValue, float aRadius, const CU::Vector2<float>& aPosition)
{
	CU::Vector2<int> topLeft(GetIntPosition(aPosition - aRadius));
	CU::Vector2<int> botRight(GetIntPosition(aPosition + aRadius));

	for (int y = topLeft.y; y < botRight.y; ++y)
	{
		for (int x = topLeft.x; x < botRight.x; ++x)
		{
			float distance = CU::Length(GetPosition(x, y) - aPosition);

			if (distance < aRadius - OUTER_RING && ValidIndex(x, y))
			{
				int index = x + y * mySide;
				myGrid[index] += aValue;
			}
			else if (distance >= aRadius - OUTER_RING && distance <= aRadius)
			{
				int index = x + y * mySide;
				float inner = aRadius - (OUTER_RING);
				float newDist = distance - inner;
				float newMax = aRadius - inner;
				float value = aValue * (1.f - (newDist / newMax));

				myGrid[index] += value;
				myGrid[index] = fminf(myGrid[index], myMaxValue);
				myGrid[index] = fmaxf(myGrid[index], MIN_DARKNESS);
			}
		}
	}
}

void FogOfWarMap::UpdateRenderPlane()
{
	myPlane->StartModify();

	/*for (int i = 0; i < myGrid.Size(); ++i)
	{
		myPlane->SetVertexColor(i, GetColor(myGrid[i]/myMaxValue));
	}*/

	myPlane->EndModify();
}

bool FogOfWarMap::IsVisible(const CU::Vector2<float>& aPosition)
{
	if (myFogEnabled == false)
	{
		return true;
	}

	return GetValue(aPosition) > MIN_DARKNESS;
}

void FogOfWarMap::ToggleFogOfWar()
{
	myFogEnabled = !myFogEnabled;
}

void FogOfWarMap::SetFogOfWarHelperTexture(Prism::Texture* aTexture)
{
	aTexture;
	//myFogOfWarHelperTexture = aTexture;
	//mySprite = Prism::ModelLoader::GetInstance()->LoadSprite(myFogOfWarHelperTexture->GetTexture(), { 600.f, 300.f }, { 0.f, 0.f });
}

void FogOfWarMap::RenderSprite()
{
	//mySprite->CopyFromD3DTexture(myFogOfWarHelperTexture->GetTexture());
	//mySprite->Render({ 500.f, 500.f });
}