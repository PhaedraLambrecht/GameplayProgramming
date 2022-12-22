#pragma once

namespace Elite
{
	template <class T_NodeType, class T_ConnectionType>
	class Dijkstra
	{
	public:
		Dijkstra(IGraph<T_NodeType, T_ConnectionType>* pGraph);

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

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
	};

	template <class T_NodeType, class T_ConnectionType>
	Dijkstra<T_NodeType, T_ConnectionType>::Dijkstra(IGraph<T_NodeType, T_ConnectionType>* pGraph)
		: m_pGraph(pGraph)
	{
	}

	template <class T_NodeType, class T_ConnectionType>
	std::vector<T_NodeType*> Dijkstra<T_NodeType, T_ConnectionType>::FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode)
	{

		//std::queue<T_NodeType*> openList{}; // queue to check the next neighbors
		//std::map<T_NodeType*, T_NodeType*> closedList{}; // connection already checked
		//
		//NodeRecord currentRecord{}; // Holds current to be evaluated
		//currentRecord.pNode = pStartNode;
		//currentRecord.pConnection = nullptr;
		//currentRecord.costSoFar = 0.0f;



		//openList.push(pDestinationNode);


		//while (!openList.empty())
		//{
		//	// keep track of current node
		//	T_NodeType* pCurrentNode{ openList.front() };
		//	openList.pop();// Delete it from the open list


		//	// If it is the destination stop
		//	if (pCurrentNode == pDestinationNode)
		//		continue;


		//	
		//	closedList.insert(pCurrentNode);// Add it to the visited list
		//	// Loop trough all the connections of the node
		//	for (auto& connection : m_pGraph->GetNodeConnections(pCurrentNode))
		//	{
		//		const auto totalCostSOFar{ currentRecord.costSoFar + connection->GetCost() };
		//		
		//		//T_NodeType* pNextNode{ m_pGraph->GetNode(connection->GetTo()) };

		//		if (totalCostSOFar < openList[connection]->costSoFar)
		//		{
		//			openList[connection]->costSoFar = totalCostSOFar;
		//			openList[connection]->previous = pCurrentNode;
		//			openList.push({ pCurrentNode, connection});
		//		}



		//	}
		//}


		//// Check if destination reached
		//if (closedList.find(pDestinationNode) == closedList.end())
		//{
		//	return std::vector<T_NodeType*>{};
		//}


		//// BackTracking
		std::vector<T_NodeType*> path{};

		//// Go trough closedList from destination -> start
		//T_NodeType* pCurrentNode{ pDestinationNode };
		//while (pCurrentNode != pStartNode)
		//{
		//	// Add current node to the path
		//	path.push_back(pCurrentNode);
		//	// Look where it came from
		//	pCurrentNode = closedList[pCurrentNode];
		//}

		//path.push_back(pStartNode);
		//// Reverse path start -> destination
		//std::reverse(path.begin(), path.end());


		return path;
		
	}
}

