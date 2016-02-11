#pragma once

#include <GrowingArray.h>
namespace Prism
{
	class Camera;
	class RenderPlane;
}

class AIMap
{
public:
	AIMap(int aSide = 64.f);
	virtual ~AIMap();

	virtual void Update(){};
	void Render(const Prism::Camera& aCamera);
	virtual void UpdateRenderPlane();

	CU::Vector2<float> GetPosition(int aIndex) const;
	CU::Vector2<int> GetIntPosition(const CU::Vector2<float>& aPosition) const;
	CU::Vector2<float> GetPosition(int aX, int aY) const;
	int GetIndex(const CU::Vector2<float>& aPosition) const;
	virtual void AddValue(float aValue, float aRadius, const CU::Vector2<float>& aPosition);
	void SetValue(int aIndex, float aValue);
	const CU::GrowingArray<float>& GetGrid() const;

	float GetValue(const CU::Vector2<float>& aPosition) const;
	CU::Vector2<float> GetMaxPosition() const;

	Prism::RenderPlane* GetRenderPlane();

protected:
	void operator=(const AIMap&) = delete;

	CU::Vector4<float> GetColor(float aValue) const;
	bool ValidIndex(int aX, int aY) const;

	const int mySide;
	const int myLevelWidth;
	float myTileSize;
	float myLevelCellSize;
	float myMaxValue;
	CU::GrowingArray<float> myGrid;

	Prism::RenderPlane* myPlane;
	CU::Matrix44<float> myOrientation;
};

inline const CU::GrowingArray<float>& AIMap::GetGrid() const
{
	return myGrid;
}

inline CU::Vector2<float> AIMap::GetMaxPosition() const
{
	DL_ASSERT("NOT IMPLEMENTED");
	return{ 0.f, 0.f };
}