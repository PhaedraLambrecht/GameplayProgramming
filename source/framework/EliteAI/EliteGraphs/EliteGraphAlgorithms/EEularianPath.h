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
			// TODO: FindPath- milestone

			// Get a copy of the graph because this algorithm involves removing edges
			auto graphCopy = m_pGraph->Clone();
			auto path = std::vector<T_NodeType*>();
			int nrOfNodes = graphCopy->GetNrOfNodes();

			// Check if there can be an Euler path
			

		/*	int nrOfConnections{};
			auto CurrentNode{};
			for (int idx{}; idx < nrOfNodes; ++idx)
			{
				CurrentNode = graphCopy[idx];
			}
			nrOfConnections = CurrentNode.GetNrOfConnections();*/


			// If this graph is not eulerian, return the empty path
			// Else we need to find a valid starting index for the algorithm
			
			// Start algorithm loop
			std::stack<int> nodeStack;


			std::reverse(path.begin(), path.end()); // reverses order of the path
			return path;

	}

	template<class T_NodeType, class T_ConnectionType>
	inline void EulerianPath<T_NodeType, T_ConnectionType>::VisitAllNodesDFS(int startIdx, std::vector<bool>& visited) const
	{
		// mark the visited node


		// recursively visit any valid connected nodes that were not visited before


		/*
				// mark the visited node
		visited[startIdx] = true;

		// recursively visit any valid connected nodes that were not visited before
		for (T_ConnectionType* connection : m_pGraph->GetNodeConnections(startIdx))
		{
			if (visited[connection->GetTo()] == false)


		}
		*/
	}

		template<class T_NodeType, class T_ConnectionType>
		inline bool EulerianPath<T_NodeType, T_ConnectionType>::IsConnected() const
		{
			auto nodes = m_pGraph->GetAllNodes();
			vector<bool> visited(m_pGraph->GetNrOfNodes(), false);


			// find a valid starting node that has connections
			if (nodes.size() > 1 && m_pGraph->GetAllConnections().size() == 0)
				return false;

			int connectionIdx = invalid_node_index;
			for (auto n : nodes)
			{
				auto connections = m_pGraph->GetNodeConnections(n);

				if (connections.size() != 0)
				{
					connectionIdx = n->GetIndex();
					break;
				}
			}


			// if no valid node could be found, return false
			if (connectionIdx == invalid_node_index)
				return false;


			// start a depth-first-search traversal from the node that has at least one connection
			VisitAllNodesDFS(connectionIdx, visited);


			// if a node was never visited, this graph is not connected
			for (auto n : nodes)
				if (visited[n->GetIndex()] == false)
					return false;


			return true;
		}

}