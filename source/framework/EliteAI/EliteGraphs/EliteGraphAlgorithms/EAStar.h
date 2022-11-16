#pragma once
#include "framework/EliteAI/EliteNavigation/ENavigation.h"

namespace Elite
{
	template <class T_NodeType, class T_ConnectionType>
	class AStar
	{
	public:
		AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction);

		// stores the optimal connection to a node and its total costs related to the start and end node of the path
		struct NodeRecord
		{
			T_NodeType* pNode = nullptr;
			T_ConnectionType* pConnection = nullptr;
			float costSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
			float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

			bool operator==(const NodeRecord& other) const
			{
				return pNode == other.pNode
					&& pConnection == other.pConnection
					&& costSoFar == other.costSoFar
					&& estimatedTotalCost == other.estimatedTotalCost;
			};

			bool operator<(const NodeRecord& other) const
			{
				return estimatedTotalCost < other.estimatedTotalCost;
			};
		};

		std::vector<T_NodeType*> FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode);

	private:
		float GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const;

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
		Heuristic m_HeuristicFunction;
	};

	template <class T_NodeType, class T_ConnectionType>
	AStar<T_NodeType, T_ConnectionType>::AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction)
		: m_pGraph(pGraph)
		, m_HeuristicFunction(hFunction)
	{
	}

	template <class T_NodeType, class T_ConnectionType>
	std::vector<T_NodeType*> AStar<T_NodeType, T_ConnectionType>::FindPath(T_NodeType* pStartNode, T_NodeType* pGoalNode)
	{
		std::vector<T_NodeType*> path{}; // Final path
		std::vector<NodeRecord> openList{}; // Connection to be checked
		std::vector<NodeRecord> closedList{}; // Connection already checked
		NodeRecord currentRecord{}; // Holds current to be evaluated


		currentRecord.pNode = pStartNode;
		currentRecord.pConnection = nullptr;
		currentRecord.costSoFar = 0.0f;
		currentRecord.estimatedTotalCost = GetHeuristicCost(pStartNode, pGoalNode);

		openList.push_back(currentRecord);


		while (!openList.empty())
		{
			// connection with lowest f
			currentRecord = *std::min_element(openList.begin(), openList.end());


			// If it is the destination, stop
			if (currentRecord.pNode == pGoalNode)
			{
				closedList.push_back(currentRecord);
				break;
			}


			//// get the nodes connections & loop over them
			for (const auto& connection : m_pGraph->GetNodeConnections(currentRecord.pNode))
			{
				// calculate the g-cost
				const auto totalCostSoFar = currentRecord.costSoFar + connection->GetCost();
				bool skipConnection{ false };


				// Check if any of those connections lead to a node already on the closed list
				for (const NodeRecord& closedListRecord : closedList)
				{
					if (connection->GetTo() == closedListRecord.pNode->GetIndex())
					{
						// Check if the already existing connection is cheaper
						if (connection->GetCost() <= totalCostSoFar)
						{
							skipConnection = true;
							continue;
						}
						closedList.erase(std::remove(closedList.begin(), closedList.end(), closedListRecord));
					}
				}

				// Check if any of those connections lead to a node already on the open list
				if (!skipConnection)
				{
					for (const NodeRecord& openListRecord : openList)
					{
						if (connection->GetTo() == openListRecord.pNode->GetIndex())
						{
							// Check if the already existing connection is cheaper
							if (connection->GetCost() <= totalCostSoFar)
							{
								continue;
							}
							openList.erase(std::remove(openList.begin(), openList.end(), openListRecord));
						}
					}
				}


				// Make new NodeRecord, and add it to the open list
				if (skipConnection == false)
				{
					NodeRecord newNodeRecord{};
					newNodeRecord.pNode = m_pGraph->GetNode(connection->GetTo());
					newNodeRecord.costSoFar = totalCostSoFar;
					newNodeRecord.pConnection = connection;
					newNodeRecord.estimatedTotalCost = totalCostSoFar + GetHeuristicCost(m_pGraph->GetNode(connection->GetTo()), pGoalNode);
					openList.push_back(newNodeRecord);
				}
			}

			// Erase current node from the open list and add it to the closed list
			openList.erase(std::remove(openList.begin(), openList.end(), currentRecord));
			closedList.push_back(currentRecord);
		}


		// Geting a fall back path
		// Reconstruct path from last connection to start node
		NodeRecord currentPathRecord = currentRecord;
		while (currentPathRecord.pNode != pStartNode)
		{
			path.push_back(currentPathRecord.pNode);

			for (const NodeRecord& nodeRecord : closedList)
			{
				// this does not work and breaks it --> this requires to not have noderecord be const
				//currentPathRecord.estimatedTotalCost = GetHeuristicCost(currentPathRecord.pNode, pGoalNode);
				//nodeRecord.estimatedTotalCost = GetHeuristicCost(nodeRecord.pNode, pGoalNode);
				//
				//if (currentPathRecord.pConnection->GetFrom() == nodeRecord.pNode->GetIndex())
				//{
				//	// If the cost of Noderecord is lower then you can change currentPath
				//	if (nodeRecord.estimatedTotalCost <= currentPathRecord.estimatedTotalCost)
				//	{
				//		currentPathRecord = nodeRecord;
				//		break;
				//	}
				//	else // else it stays the same
				//	{
				//		currentPathRecord = currentPathRecord;
				//		break;
				//	}
				//}


				if (currentPathRecord.pConnection->GetFrom() == nodeRecord.pNode->GetIndex())
				{
					currentPathRecord = nodeRecord;
					break;
				}
			}
		}



		// Reverse the path, don't forget the startNode
		path.push_back(pStartNode);
		std::reverse(path.begin(), path.end());

		//check if path doesn't lead to end point
		if (currentRecord.pNode != pGoalNode)
		{
			return path;
		}


		return path;
	}

	template <class T_NodeType, class T_ConnectionType>
	float Elite::AStar<T_NodeType, T_ConnectionType>::GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const
	{
		Vector2 toDestination = m_pGraph->GetNodePos(pEndNode) - m_pGraph->GetNodePos(pStartNode);
		return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
	}
}