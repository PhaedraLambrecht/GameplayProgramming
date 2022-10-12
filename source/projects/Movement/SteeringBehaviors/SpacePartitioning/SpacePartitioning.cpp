#include "stdafx.h"
#include "SpacePartitioning.h"
#include "projects\Movement\SteeringBehaviors\SteeringAgent.h"

// --- Cell ---
// ------------
Cell::Cell(float left, float bottom, float width, float height)
{
	boundingBox.bottomLeft = { left, bottom };
	boundingBox.width = width;
	boundingBox.height = height;
}

std::vector<Elite::Vector2> Cell::GetRectPoints() const
{
	auto left = boundingBox.bottomLeft.x;
	auto bottom = boundingBox.bottomLeft.y;
	auto width = boundingBox.width;
	auto height = boundingBox.height;

	std::vector<Elite::Vector2> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(float width, float height, int rows, int cols, int maxEntities)
	: m_SpaceWidth(width)
	, m_SpaceHeight(height)
	, m_NrOfRows(rows)
	, m_NrOfCols(cols)
	, m_Neighbors(maxEntities)
	, m_NrOfNeighbors(0)
{
	float left{};
	float bottom{};
	m_CellWidth = m_SpaceWidth / m_NrOfCols;
	m_CellHeight = m_SpaceHeight / m_NrOfRows;

	for(int rowIdx{}; rowIdx < m_NrOfRows; ++rowIdx)
	{
		left = 0.0f; // reset left to 0

		for (int colsIdx{}; colsIdx < m_NrOfCols; ++colsIdx)
		{
			m_Cells.push_back(Cell(left, bottom, m_CellWidth, m_CellHeight));

			left += m_CellWidth; // += with the cells width
		}

		bottom += m_CellHeight; // += with the cells height
	}


}

void CellSpace::AddAgent(SteeringAgent* agent)
{
	m_Cells[PositionToIndex(agent->GetPosition())].agents.push_back(agent);
}

void CellSpace::UpdateAgentCell(SteeringAgent* agent, Elite::Vector2 oldPos)
{
	int oldCellIdx{ PositionToIndex(oldPos) };
	int newCellIdx{ PositionToIndex(agent->GetPosition()) };


	// Check if they aren't the same
	if( oldCellIdx != newCellIdx )
	{
		// delete agent from old cell
		m_Cells[oldCellIdx].agents.remove(agent);


		// add agent to new cell
		m_Cells[newCellIdx].agents.push_back(agent);
	}
	

}

void CellSpace::RegisterNeighbors(SteeringAgent* agent, float queryRadius)
{
	// TODO: - RegisterNeighbors
}

void CellSpace::EmptyCells()
{
	for (Cell& c : m_Cells)
		c.agents.clear();
}

void CellSpace::RenderCells() const
{
	for(size_t idx{}; idx < (m_NrOfCols * m_NrOfRows); ++idx)
	{
		const std::vector <Elite::Vector2> cellRect = m_Cells[idx].GetRectPoints();

		DEBUGRENDERER2D->DrawSegment(cellRect[0], cellRect[1], { 1.0f, 0.0f, 0.0f });
		DEBUGRENDERER2D->DrawSegment(cellRect[1], cellRect[2], { 1.0f, 0.0f, 0.0f });
		DEBUGRENDERER2D->DrawSegment(cellRect[2], cellRect[3], { 1.0f, 0.0f, 0.0f });
		DEBUGRENDERER2D->DrawSegment(cellRect[0], cellRect[3], { 1.0f, 0.0f, 0.0f });








	}

}

int CellSpace::PositionToIndex(const Elite::Vector2 pos) const
{
	//// checking for pos x
	//if(pos.x <0)
	//{
	//	return 0;
	//}
	//if(pos.x > m_NrOfCols)
	//{
	//	return m_NrOfCols / m_NrOfRows;
	//}
	//
	//
	//// checking for pos y
	//if(pos.y < 0)
	//{
	//	return 0;
	//}
	//if(pos.y > m_NrOfRows)
	//{
	//	return m_NrOfRows / m_NrOfCols;
	//}


	int finalIdx{};

	int Tx = pos.x / m_CellWidth;
	int Ty = pos.y / m_CellHeight;

	finalIdx = m_NrOfCols * Ty + Tx;


	return finalIdx;
}