#include "stdafx.h"
#include "NavMesh.h"
#include "PathFinderAStar.h"
#include "Edge.h"

#include <sstream>


namespace Prism
{

	namespace Navigation
	{
		PathFinderAStar::PathFinderAStar(NavMesh* aNavMesh)
			: myNavMesh(aNavMesh)
			, myHeuristicMult(10.f)
			, myStart(nullptr)
			, myEnd(nullptr)
			, myPathLength(0.f)
		{
		}

		PathFinderAStar::~PathFinderAStar()
		{
		}

		bool PathFinderAStar::FindPath(const CU::Vector3<float>& aStart, const CU::Vector3<float>& anEnd
			, CU::GrowingArray<Triangle*>& someTrianglesOut)
		{
			return FindPath({ aStart.x, aStart.z }, { anEnd.x, anEnd.z }, someTrianglesOut);
		}

		bool PathFinderAStar::FindPath(const CU::Vector2<float>& aStart
			, const CU::Vector2<float>& anEnd, CU::GrowingArray<Triangle*>& someTrianglesOut)
		{
			myStart = nullptr;
			myEnd = nullptr;

			if (FindStartAndEnd(aStart, anEnd) == true)
			{
				someTrianglesOut.RemoveAll();
				ClearMesh();
				AStar();
				FillPath(someTrianglesOut);

				std::stringstream ss;
				ss << "Triangles: " << someTrianglesOut.Size() << std::endl;
				if (someTrianglesOut.Size() > 0)
				{
					ss << "Distance: " << someTrianglesOut[0]->myCurrDist;
				}
				DEBUG_PRINT(ss.str());

				myLatestPath = someTrianglesOut;

				return true;
			}
			DEBUG_PRINT("no path found");
			DEBUG_PRINT(aStart);
			DEBUG_PRINT(anEnd);

			return false;
		}

		void PathFinderAStar::Render()
		{
		}

		bool PathFinderAStar::IsOutside(const CU::Vector2<float>& aPosition)
		{
			return GetCurrentTriangle(aPosition) == nullptr;
		}

		Triangle* PathFinderAStar::GetCurrentTriangle(const CU::Vector2<float>& aPosition)
		{
			const CU::GrowingArray<Triangle*>& triangles = myNavMesh->GetTriangles();
			for (int i = 0; i < triangles.Size(); ++i)
			{
				Triangle* current = myNavMesh->GetTriangles()[i];
				if (current->Collide(aPosition) == true)
				{
					return current;
				}
			}

			return nullptr;
		}

		bool PathFinderAStar::FindStartAndEnd(const CU::Vector2<float>& aStart, const CU::Vector2<float>& anEnd)
		{
			bool foundOne(false);
			bool foundTwo(false);

			const CU::GrowingArray<Triangle*>& triangles = myNavMesh->GetTriangles();
			for (int i = 0; i < triangles.Size(); ++i)
			{
				Triangle* current = myNavMesh->GetTriangles()[i];
				if (current->Collide(aStart) == true)
				{
					//myText->myText = std::to_string(i);
					myStart = current;
					if (foundOne == true)
					{
						foundTwo = true;
						break;
					}
					foundOne = true;
				}
				if (current->Collide(anEnd) == true)
				{
					myEnd = current;
					if (foundOne == true)
					{
						foundTwo = true;
						break;
					}
					foundOne = true;
				}
			}

			return foundTwo;
		}

		void PathFinderAStar::ClearMesh()
		{
			myPathLength = -1.f;

			const CU::GrowingArray<Triangle*>& triangles = myNavMesh->GetTriangles();
			for (int i = 0; i < triangles.Size(); ++i)
			{
				triangles[i]->myType = Triangle::eType::WALKABLE;
				triangles[i]->myState = Triangle::eState::NONE;
			}
		}

		void PathFinderAStar::AStar()
		{
			CU::Heap<Triangle*, LesserTriangle<Triangle*>> toBeChecked;

			const CU::GrowingArray<Triangle*>& triangles = myNavMesh->GetTriangles();
			for (int i = 0; i < triangles.Size(); ++i)
			{
				Triangle* t = triangles[i];
				if (t != myStart)
				{
					//t.myPos.x = x;
					//t.myPos.y = y;
					t->myCurrDist = FLT_MAX;
					t->myCostToGoal = 0;
					t->myTotalCost = t->myCurrDist + t->myCostToGoal;
					t->myChecked = false;
					t->myPredec = nullptr;
				}
			}

			//Vertex& start(myBoard[myStart->myPos.x][myStart->myPos.y]);
			//myStart->myPos.x = myStart->myPos.x;
			//myStart->myPos.y = myStart->myPos.y;
			myStart->myCurrDist = 0;
			myStart->myCostToGoal = CostEstimate(myStart, myEnd);
			myStart->myChecked = false;

			toBeChecked.Enqueue(myStart);
			myStart->myState = Triangle::eState::OPEN;

			while (toBeChecked.IsEmpty() == false)
			{
				Triangle* t = toBeChecked.Dequeue();

				t->myState = Triangle::eState::NONE;

				if (t == myEnd)
				{
					break;
				}
				else
				{
					//check all available triangles

					for (int i = 1; i <= 3; ++i)
					{
						Triangle* tToCheck(nullptr);
						if (i == 1)
						{
							tToCheck = t->GetOtherTriangle(t->myEdge1);
							if (tToCheck == nullptr)// || tToCheck->myArea < 1.f)
							{
								continue;
							}
						}
						else if (i == 2)
						{
							tToCheck = t->GetOtherTriangle(t->myEdge2);
							if (tToCheck == nullptr)// || tToCheck->myArea < 1.f)
							{
								continue;
							}
						}
						else if (i == 3)
						{
							tToCheck = t->GetOtherTriangle(t->myEdge3);
							if (tToCheck == nullptr)// || tToCheck->myArea < 1.f)
							{
								continue;
							}
						}

						float newCost = t->myCurrDist + CostEstimate(t, tToCheck);

						if ((tToCheck->myState == Triangle::eState::OPEN
							|| tToCheck->myState == Triangle::eState::CLOSED)
							&& (tToCheck->myCurrDist <= newCost))
						{
							continue;
						}
						else
						{
							tToCheck->myPredec = t;
							tToCheck->myCurrDist = newCost;
							tToCheck->myCostToGoal = CostEstimate(tToCheck, myEnd);
							tToCheck->myTotalCost = tToCheck->myCurrDist + tToCheck->myCostToGoal;
						}

						if (tToCheck->myState == Triangle::eState::OPEN)
						{
							toBeChecked.Heapify();
						}
						else
						{
							tToCheck->myState = Triangle::eState::OPEN;
							toBeChecked.Enqueue(tToCheck);
						}
					}
				}

				t->myState = Triangle::eState::CLOSED;
			}
		}

		void PathFinderAStar::FillPath(CU::GrowingArray<Triangle*>& someTrianglesOut) const
		{
			Triangle* current = myEnd;

			int max = myNavMesh->GetTriangles().Size();
			int iteration = 0;
			while (current != myStart && iteration < max)
			{
				++iteration;
				someTrianglesOut.Add(current);
				current = current->myPredec;
			}
			someTrianglesOut.Add(current);
		}
	}
}