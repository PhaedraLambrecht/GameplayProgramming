#include "stdafx.h"
#include "ENavGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAStar.h"

using namespace Elite;

Elite::NavGraph::NavGraph(const Polygon& contourMesh, float playerRadius = 1.0f) :
	Graph2D(false),
	m_pNavMeshPolygon(nullptr)
{
	//Create the navigation mesh (polygon of navigatable area= Contour - Static Shapes)
	m_pNavMeshPolygon = new Polygon(contourMesh); // Create copy on heap

	//Get all shapes from all static rigidbodies with NavigationCollider flag
	auto vShapes = PHYSICSWORLD->GetAllStaticShapesInWorld(PhysicsFlags::NavigationCollider);

	//Store all children
	for (auto shape : vShapes)
	{
		shape.ExpandShape(playerRadius);
		m_pNavMeshPolygon->AddChild(shape);
	}

	//Triangulate
	m_pNavMeshPolygon->Triangulate();

	//Create the actual graph (nodes & connections) from the navigation mesh
	CreateNavigationGraph();
}

Elite::NavGraph::~NavGraph()
{
	delete m_pNavMeshPolygon;
	m_pNavMeshPolygon = nullptr;
}

int Elite::NavGraph::GetNodeIdxFromLineIdx(int lineIdx) const
{
	auto nodeIt = std::find_if(m_Nodes.begin(), m_Nodes.end(), [lineIdx](const NavGraphNode* n) { return n->GetLineIndex() == lineIdx; });
	if (nodeIt != m_Nodes.end())
	{
		return (*nodeIt)->GetIndex();
	}

	return invalid_node_index;
}

Elite::Polygon* Elite::NavGraph::GetNavMeshPolygon() const
{
	return m_pNavMeshPolygon;
}

void Elite::NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigationmesh and create nodes
	//Loop over all the lines
	for (const auto& line : m_pNavMeshPolygon->GetLines())
	{
		// Check if that line is connected to another triangle 
		const auto trianglesFromLineIdx{ m_pNavMeshPolygon->GetTrianglesFromLineIndex(line->index) };

		if ( !(trianglesFromLineIdx.size() >= 2) )
			continue;


		// Create a NavGraphNode on the graph
		const Elite::Vector2 newNodePos{ (line->p1 + line->p2) / 2.0f };
		NavGraphNode* newNavGraphNode{ new NavGraphNode{GetNextFreeNodeIndex(), line->index, newNodePos} };
		this->AddNode(newNavGraphNode);

	}


	//// Loop over all the triangles
	for (const auto& triangle : m_pNavMeshPolygon->GetTriangles()) // loop over all triangles
	{
		std::vector<int> triangleNodeIdx; // Used for connection


		for (const auto& lineIndex : triangle->metaData.IndexLines)
		{
			auto nodeIdx{ GetNodeIdxFromLineIdx(lineIndex) };

			if (nodeIdx == invalid_node_index)
				continue;


			triangleNodeIdx.push_back(nodeIdx);
		}


		//2. Create connections now that every node is created
		GraphConnection2D* newConnection{ nullptr };

		switch (triangleNodeIdx.size())
		{
		case 2:

			newConnection = new GraphConnection2D(triangleNodeIdx[0], triangleNodeIdx[1]);
			this->AddConnection(newConnection);

			break;
		case 3:

			newConnection = new GraphConnection2D(triangleNodeIdx[0], triangleNodeIdx[1]);
			this->AddConnection(newConnection);

			newConnection = new GraphConnection2D(triangleNodeIdx[1], triangleNodeIdx[2]);
			this->AddConnection(newConnection);

			newConnection = new GraphConnection2D(triangleNodeIdx[2], triangleNodeIdx[0]);
			this->AddConnection(newConnection);

			break;
		}
	}


	//3. Set the connections cost to the actual distance
	this->SetConnectionCostsToDistance();


}

