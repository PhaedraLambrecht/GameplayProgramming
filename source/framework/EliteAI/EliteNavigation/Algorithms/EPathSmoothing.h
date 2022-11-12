#pragma once

#include <vector>
#include "framework/EliteGeometry/EGeometry2DTypes.h"
#include "framework/EliteAI/EliteGraphs/EGraphNodeTypes.h"

namespace Elite
{
	//Portal struct (only contains line info atm, you can expand this if needed)
	struct Portal
	{
		Portal() {}
		explicit Portal(const Elite::Line& line) :
			Line(line)
		{
		}
		Elite::Line Line = {};
	};


	class SSFA final
	{
	public:
		//=== SSFA Functions ===
		//--- References ---
		//http://digestingduck.blogspot.be/2010/03/simple-stupid-funnel-algorithm.html
		//https://gamedev.stackexchange.com/questions/68302/how-does-the-simple-stupid-funnel-algorithm-work
		static std::vector<Portal> FindPortals(
			const std::vector<NavGraphNode*>& nodePath,
			Polygon* navMeshPolygon)
		{
			//Container
			std::vector<Portal> vPortals = {};

			vPortals.push_back(Portal(Line(nodePath[0]->GetPosition(), nodePath[0]->GetPosition())));

			std::vector<Line*> lines = navMeshPolygon->GetLines();

			//For each node received, get its corresponding line
			for (size_t nodeIdx = 1; nodeIdx < nodePath.size() - 1; ++nodeIdx)
			{
				//Local variables
				NavGraphNode* pNode = nodePath[nodeIdx]; //Store node, except last node, because this is our target node!
				Line* pLine = lines[pNode->GetLineIndex()];

				//Redetermine it's "orientation" based on the required path (left-right vs right-left) - p1 should be right point
				Vector2 centerLine = (pLine->p1 + pLine->p2) / 2.0f;
				Vector2 previousPosition = nodeIdx == 0 ? nodePath[0]->GetPosition() : nodePath[nodeIdx - 1]->GetPosition();

				float cross = Cross((centerLine - previousPosition), (pLine->p1 - previousPosition));

				Line portalLine = {};
				if (cross > 0)//Left
					portalLine = Line(pLine->p2, pLine->p1);
				else //Right
					portalLine = Line(pLine->p1, pLine->p2);

				//Store portal
				vPortals.push_back(Portal(portalLine));
			}
			//Add degenerate portal to force end evaluation
			vPortals.push_back(Portal(Line(nodePath[nodePath.size() - 1]->GetPosition(), nodePath[nodePath.size() - 1]->GetPosition())));

			return vPortals;
		}

		static std::vector<Vector2> OptimizePortals(const std::vector<Portal>& portals)
		{
			//P1 == right point of portal, P2 == left point of portal
			std::vector<Vector2> vPath = {};
			const unsigned int amtPortals{ static_cast<unsigned int>(portals.size()) };

			int apexIdx{ 0 }, leftLegIdx{ 1 }, rightLegIdx{ 1 };

			Vector2 apexPos = portals[apexIdx].Line.p1;
			Vector2 rightLeg = portals[rightLegIdx].Line.p1 - apexPos;
			Vector2 leftLeg = portals[leftLegIdx].Line.p2 - apexPos;

			for (unsigned int portalIdx = 1; portalIdx < amtPortals; ++portalIdx)
			{
				//Local
				const auto& portal = portals[portalIdx];
				


				//--- RIGHT CHECK ---
				Vector2 newRightLeg{ portal.Line.p1 - apexPos };
				
				//1. See if moving funnel inwards - RIGHT
				auto rightCross{ Elite::Cross(newRightLeg, rightLeg) };
				
				if (rightCross <= 0.0f)// Going inwards
				{
					//2. See if new line degenerates a line segment - RIGHT
					auto crossingLeftCross{ Elite::Cross(newRightLeg, leftLeg) };

					if (crossingLeftCross <= 0.0f) // it crosses the left leg
					{
						apexPos += leftLeg;
						apexIdx = leftLegIdx;
						portalIdx = leftLegIdx + 1;


						// New itterator
						auto newIterrator { apexIdx + 1 };
						portalIdx = newIterrator;
						leftLegIdx = newIterrator;
						rightLegIdx = newIterrator;

						vPath.push_back(apexPos);
						

						// Using the new iterators calculate the new legs if it smaller than the size of the portals 
						if (newIterrator < amtPortals)
						{
							rightLeg = portals[rightLegIdx].Line.p1 - apexPos;
							leftLeg = portals[leftLegIdx].Line.p2 - apexPos;
							continue;
						}
					}
					else
					{
						rightLeg = newRightLeg;
						rightLegIdx = portalIdx;
					}
				}




				//--- LEFT CHECK ---
				auto newLeftLeg{ portal.Line.p2 - apexPos };
				
				//1. See if moving funnel inwards - LEFT
				auto leftCross{ Elite::Cross(newLeftLeg, leftLeg) };

				if (leftCross <= 0.0f)// Going inwards
				{
					//2. See if new line degenerates a line segment - LEFT
					auto crossingRightCross{ Elite::Cross(newLeftLeg, rightLeg) };

					if (crossingRightCross <= 0.0f) // it crosses the right leg
					{
						apexPos += rightLeg;
						apexIdx = rightLegIdx;
						portalIdx = rightLegIdx + 1;


						// New itterator
						auto newIterrator{ apexIdx + 1 };
						portalIdx = newIterrator;
						leftLegIdx = newIterrator;
						rightLegIdx = newIterrator;

						vPath.push_back(apexPos);


						// Using the new iterators calculate the new legs if it smaller than the size of the portals 
						if (newIterrator < amtPortals)
						{
							rightLeg = portals[rightLegIdx].Line.p1 - apexPos;
							leftLeg = portals[leftLegIdx].Line.p2 - apexPos;
							continue;
						}
					}
					else
					{
						leftLeg = newLeftLeg;
						leftLegIdx = portalIdx;
					}
				}

					

			}

			// Add last path point (You can use the last portal p1 or p2 points as both are equal to the endPoint of the path
			vPath.push_back(portals[portals.size() - 1].Line.p1);


			return vPath;
		}
	private:
		SSFA() {};
		~SSFA() {};
	};
}
