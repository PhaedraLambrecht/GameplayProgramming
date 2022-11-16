#pragma once
#include <stack>

namespace Elite
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	template <class T_NodeType, class T_ConnectionType>
	class EulerianPath
	{
	public:

		EulerianPath(IGraph<T_NodeType, T_ConnectionType>* pGraph);

		Eulerianity IsEulerian() const;
		std::vector<T_NodeType*> FindPath(Eulerianity& eulerianity) const;

	private:
		void VisitAllNodesDFS(int startIdx, std::vector<bool>& visited) const;
		bool IsConnected() const;

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
	};

	template<class T_NodeType, class T_ConnectionType>
	inline EulerianPath<T_NodeType, T_ConnectionType>::EulerianPath(IGraph<T_NodeType, T_ConnectionType>* pGraph)
		: m_pGraph(pGraph)
	{
	}

	template<class T_NodeType, class T_ConnectionType>
	inline Eulerianity EulerianPath<T_NodeType, T_ConnectionType>::IsEulerian() const
	{
		// Not connected --> not eulerian
		if (IsConnected() == false)
			return Eulerianity::notEulerian;


		// Count the nodes
		int nodeCount{};
		for (const auto& node : m_pGraph->GetAllNodes())
		{
			 auto nodeConnections{ m_pGraph->GetNodeConnections(node) };

			if (nodeConnections.size() % 2 == 1)
				++nodeCount;
		}


		// Connected graph with....

		// More than 2 nodes & an odd degree --> not eulerian
		if (nodeCount > 2)
			return Eulerianity::notEulerian;


		// Exactly 2 nodes & an odd degree --> semi-eulerian
		if (nodeCount == 2)
			return Eulerianity::semiEulerian;


		// No odd nodes -->  Eulerian
		return Eulerianity::eulerian;
	}

	template<class T_NodeType, class T_ConnectionType>
	inline std::vector<T_NodeType*> EulerianPath<T_NodeType, T_ConnectionType>::FindPath(Eulerianity& eulerianity) const
	{
			// Get a copy of the graph because this algorithm involves removing edges
			auto graphCopy = m_pGraph->Clone();
			auto path = std::vector<T_NodeType*>();
			int nrOfNodes = graphCopy->GetNrOfNodes();


			// Check if there can be an Euler path
			// If this graph is not eulerian, return the empty path
			// Else we need to find a valid starting index for the algorithm
			T_NodeType* currentNode{ nullptr };

			switch (eulerianity)
			{
			case Eulerianity::notEulerian:

				return path;

				break;
			case Eulerianity::semiEulerian:

				for (const auto& node : graphCopy->GetAllNodes())
				{
					const auto& connection{ graphCopy->GetNodeConnections(node) };
						if (connection.size() % 2 == 1)
							currentNode = node;

					break;
				}

				break;
			case Eulerianity::eulerian:

				currentNode = m_pGraph->GetAllNodes()[0];

				break;
			}


			// Start algorithm loop
			std::stack<int> nodeStack;

			// while graph size != 0  OR  nodeStack size > 0
			while ( (graphCopy->GetNodeConnections(currentNode).size() != 0) || (nodeStack.size() > 0) )
			{
				// if the graph size == 0
				if (graphCopy->GetNodeConnections(currentNode).size() == 0)
				{
					path.push_back(currentNode);
					currentNode = graphCopy->GetNode(nodeStack.top());
					nodeStack.pop();
				}
				else
				{
					nodeStack.push(currentNode->GetIndex());
					currentNode = graphCopy->GetNode(graphCopy->GetNodeConnections(currentNode).front()->GetTo());
					graphCopy->RemoveConnection(nodeStack.top(), currentNode->GetIndex());
				}

			}

			

			path.push_back(currentNode);
			return path;
	}

	template<class T_NodeType, class T_ConnectionType>
	inline void EulerianPath<T_NodeType, T_ConnectionType>::VisitAllNodesDFS(int startIdx, std::vector<bool>& visited) const
	{
		// mark the visited node
		visited[startIdx] = true;


		// recursively visit any valid connected nodes that were not visited before
		for (const auto& connection : m_pGraph->GetNodeConnections(startIdx))
			if (visited[connection->GetTo()] == false)
				VisitAllNodesDFS(connection->GetTo(), visited);
	}

		template<class T_NodeType, class T_ConnectionType>
		inline bool EulerianPath<T_NodeType, T_ConnectionType>::IsConnected() const
		{
			const auto& connection{ m_pGraph->GetAllNodes() };
			vector<bool> visited(m_pGraph->GetNrOfNodes(), false);
		

			// find a valid starting node that has connections
			if (connection.size() > 1 && m_pGraph->GetAllConnections().size() == 0)
				return false;


			int connectionIdx = invalid_node_index;
			for (const auto& node : connection)
			{
				const auto& connections = m_pGraph->GetNodeConnections(node);

				if (connections.size() != 0)
				{
					connectionIdx = node->GetIndex();
					break;
				}
				else
					return false;
			}


			// if no valid node could be found, return false
			if (connectionIdx == invalid_node_index)
				return false;


			// start a depth-first-search traversal from the node that has at least one connection
			VisitAllNodesDFS(connectionIdx, visited);


			// if a node was never visited, this graph is not connected
			for (const auto& n : connection)
				if (visited[n->GetIndex()] == false)
					return false;


			return true;
		}

}