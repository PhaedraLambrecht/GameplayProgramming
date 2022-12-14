#pragma once

namespace Elite 
{
	template <class T_NodeType, class T_ConnectionType>
	class BFS
	{
	public:
		BFS(IGraph<T_NodeType, T_ConnectionType>* pGraph);

		std::vector<T_NodeType*> FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode);
	private:
		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
	};

	template <class T_NodeType, class T_ConnectionType>
	BFS<T_NodeType, T_ConnectionType>::BFS(IGraph<T_NodeType, T_ConnectionType>* pGraph)
		: m_pGraph(pGraph)
	{
	}

	template <class T_NodeType, class T_ConnectionType>
	std::vector<T_NodeType*> BFS<T_NodeType, T_ConnectionType>::FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode)
	{

		std::queue<T_NodeType*> openList{}; // deque to check the next neighbors
		std::map<T_NodeType*, T_NodeType*> closedList{};// Already visited nodes

		openList.push(pStartNode);

		


		while (openList.empty() == false)
		{
			// keep track of current node
			T_NodeType* pCurrentNode{ openList.front() };
			openList.pop();// Delete it from the open list


			// If it is the destination stop
			if(pCurrentNode == pDestinationNode)
				break;
			

			// Loop trough all the connections of the node
			for (auto& connection: m_pGraph->GetNodeConnections(pCurrentNode))
			{
				T_NodeType* pNextNode{ m_pGraph->GetNode(connection->GetTo()) };


				// Check if node is already visited
				if( closedList.find(pNextNode) == closedList.end() )
				{
					openList.push(pNextNode);
					closedList[pNextNode] = pCurrentNode; // When done put current Node into the closed list
				}
			}
		}


		// Check if destination reached
		if(closedList.find(pDestinationNode) == closedList.end())
		{
			return std::vector<T_NodeType*>{};
		}



		// BackTracking

		std::vector<T_NodeType*> path{};

		// Go trough closedList from destination -> start
		T_NodeType* pCurrentNode{ pDestinationNode };
		while (pCurrentNode != pStartNode)
		{
			// Add current node to the path
			path.push_back(pCurrentNode);
			// Look where it came from
			pCurrentNode = closedList[pCurrentNode];
		}

		path.push_back(pStartNode);
		// Reverse path start -> destination
		std::reverse(path.begin(), path.end());


		return path;
	}
}

