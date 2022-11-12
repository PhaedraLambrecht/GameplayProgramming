#pragma once
#include <vector>
#include <iostream>
#include "framework/EliteMath/EMath.h"
#include "framework\EliteAI\EliteGraphs\ENavGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAStar.h"

namespace Elite
{
	class NavMeshPathfinding
	{
	public:
		static std::vector<Vector2> FindPath(Vector2 startPos, Vector2 endPos, NavGraph* pNavGraph, std::vector<Vector2>& debugNodePositions, std::vector<Portal>& debugPortals)
		{
			//Create the path to return
			std::vector<Vector2> finalPath{};

			//Get the start and endTriangle
			const auto startTriangle{ pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(startPos) };
			const auto endTriangle{ pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(endPos) };

			if (startTriangle == endTriangle)
			{
				finalPath.push_back(endPos);
				return finalPath;
			}


			//We have valid start/end triangles and they are not the same
			//=> Start looking for a path
			//Copy the graph
			auto clonedGraph = pNavGraph->Clone();
			auto clonedGraphGet = clonedGraph.get();


			//Create extra node for the Start Node (Agent's position)
			const auto startNextFreeNodeIdx{ clonedGraphGet->GetNextFreeNodeIndex() };

			NavGraphNode* pStartNode{ new NavGraphNode{startNextFreeNodeIdx, -1, startPos} };
			clonedGraphGet->AddNode(pStartNode);

			// Loop over all the edges of the start triangle 
			for (const auto& lineIdx : startTriangle->metaData.IndexLines)
			{
				auto nodeIdx{ pNavGraph->GetNodeIdxFromLineIdx(lineIdx) };

				if (nodeIdx == invalid_node_index)
					continue;


				// create and add connection
				const auto distance{ Distance(pStartNode->GetPosition(), pNavGraph->GetNode(nodeIdx)->GetPosition()) };
				GraphConnection2D* connection{ new GraphConnection2D {
																		pStartNode->GetIndex(),
																		nodeIdx,
																		distance
																	 }
											 };

				clonedGraphGet->AddConnection(connection);
			}


			//Create extra node for the endNode
			const auto endNextFreeNodeIdx{ clonedGraphGet->GetNextFreeNodeIndex() };

			NavGraphNode* pEndNode{ new NavGraphNode{endNextFreeNodeIdx, -1, endPos} };
			clonedGraphGet->AddNode(pEndNode);

			// Loop over all the edges of the start triangle 
			for (const auto& lineIdx : endTriangle->metaData.IndexLines)
			{
				auto nodeIdx{ pNavGraph->GetNodeIdxFromLineIdx(lineIdx) };

				if (nodeIdx == invalid_node_index)
					continue;


				// create and add connection
				const auto distance{ Distance(pEndNode->GetPosition(), pNavGraph->GetNode(nodeIdx)->GetPosition()) };
				GraphConnection2D* connection{ new GraphConnection2D {
																		pEndNode->GetIndex(),
																		nodeIdx,
																		distance
																	 }
				};

				clonedGraphGet->AddConnection(connection);
			}


			//Run A star on new graph
			auto pathfinder = AStar<NavGraphNode, GraphConnection2D>(clonedGraphGet, HeuristicFunctions::Chebyshev);
			auto path = pathfinder.FindPath(pStartNode, pEndNode);
			
			finalPath.reserve(path.size());
			finalPath.resize(path.size());
			for (size_t idx = 0; idx < path.size(); ++idx)
			{
				finalPath[idx] = path[idx]->GetPosition();
			}

			
			//OPTIONAL BUT ADVICED: Debug Visualisation

			debugNodePositions.clear();
			debugNodePositions.reserve(debugNodePositions.size() + finalPath.size());
			for (const Elite::Vector2& nodePos : finalPath)
			{
				debugNodePositions.push_back(nodePos);
			}


			//Run optimiser on new graph, MAKE SURE the A star path is working properly before starting this section and uncommenting this!!!
			auto portals = SSFA::FindPortals(path, pNavGraph->GetNavMeshPolygon());
			finalPath = SSFA::OptimizePortals(portals);


			return finalPath;
		}
	};
}
