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
	, m_CellWidth{width / cols}
	, m_CellHeight{height / rows}
{

	InitializeGrid();
}

void CellSpace::InitializeGrid()
{
	float left{};
	float bottom{};

	// add cells
	for (int rowIdx{}; rowIdx < m_NrOfRows; ++rowIdx)
	{
		bottom = m_CellHeight * rowIdx; // += with the cells height

		for (int colsIdx{}; colsIdx < m_NrOfCols; ++colsIdx)
		{
			left = m_CellWidth * colsIdx; // += with the cells width

			m_Cells.push_back(Cell(left, bottom, m_CellWidth, m_CellHeight));
		}
	}
}


void CellSpace::AddAgent(SteeringAgent* pAgent)
{
	int idx{ PositionToIndex(pAgent->GetPosition()) };
	m_Cells[idx].agents.push_back(pAgent);
}

void CellSpace::UpdateAgentCell(SteeringAgent* pAgent, Elite::Vector2 oldPos)
{
	int oldCellIdx{ PositionToIndex(oldPos) };
	int newCellIdx{ PositionToIndex(pAgent->GetPosition()) };


	// Check if they aren't the same
	if( oldCellIdx != newCellIdx )
	{
		m_Cells[oldCellIdx].agents.remove(pAgent); // Delete pAgent from old cell
		m_Cells[newCellIdx].agents.push_back(pAgent); // Add pAgent to new cell
	}
}

void CellSpace::RegisterNeighbors(SteeringAgent* pAgent, float queryRadius)
{
	m_NrOfNeighbors = 0;// set neighbor count to 0

	const Elite::Vector2 agentPosition{ pAgent->GetPosition() };


	// Gets the start and end index
	Elite::Vector2 leftbottomBindingBox
	{
		agentPosition.x - queryRadius,
		agentPosition.y - queryRadius
	};
	Elite::Vector2 rightTopBindingBox
	{
		agentPosition.x + queryRadius,
		agentPosition.y + queryRadius
	};

	// Get the Positions for max/min row/column
	int maxRow{ PositionToRow(rightTopBindingBox) };
	int maxCol{ PositionToCol(rightTopBindingBox) };
	int minRow{ PositionToRow(leftbottomBindingBox) };
	int minCol{ PositionToCol(leftbottomBindingBox) };


	// amount of cols for neighborhood square
	int amountCols = (queryRadius * 2) / m_CellWidth;


	for(int row{minRow}; row <= maxRow; ++row)
	{
		for (int col{ minCol }; col <= maxCol; ++col)
		{
			// rendering the active cells
			if (pAgent->CanRenderBehavior())
			{
				auto rectPoints{ m_Cells[row * m_NrOfCols + col].GetRectPoints() };
				DEBUGRENDERER2D->DrawPolygon(rectPoints.data(), rectPoints.size(), { 0.0f, 0.0f, 1.0f }, 0.0f);
			}


			int cellIndex{ row * amountCols + col };
			// puts agents in the neighborhood vector
			for (const auto& cellAgent : m_Cells[cellIndex].agents)
			{
				m_Neighbors[m_NrOfNeighbors] = cellAgent;
				++m_NrOfNeighbors;
			}
		}
	}

}

void CellSpace::EmptyCells()
{
	for (Cell& c : m_Cells)
		c.agents.clear();
}

void CellSpace::RenderCells() const
{
	// loop trough all the cells
	for (const Cell& cell : m_Cells)
	{
		const std::vector <Elite::Vector2> cellRect{ cell.GetRectPoints() };

		// Draws the  cell
		DEBUGRENDERER2D->DrawPolygon(cellRect.data(), cellRect.size(), { 1.0f, 0.0f, 0.f }, 0.0f);


		// num of agents
		std::string agentCount{ std::to_string(cell.agents.size()) };
		DEBUGRENDERER2D->DrawString({ cellRect[1].x, cellRect[1].y }, agentCount.c_str());
	}
}


int CellSpace::PositionToIndex(const Elite::Vector2 pos) const
{
	int finalIdx{};

	int colIdx = pos.x / m_CellWidth;
	int rowIdx = pos.y / m_CellHeight;


	// clamping the max and min positions
	if (colIdx < 0)
	{
		colIdx = 0;
	}
	else if(colIdx >= m_NrOfCols)
	{
		colIdx = m_NrOfRows - 1;
	}
	if(rowIdx < 0)
	{
		rowIdx = 0;
	}
	else if(rowIdx >= m_NrOfRows)
	{
		rowIdx = m_NrOfRows - 1;
	}


	finalIdx = m_NrOfCols * rowIdx + colIdx;


	return finalIdx;
}

int CellSpace::PositionToCol(const Elite::Vector2 & pos) const
{
	int colIdx = pos.x / m_CellWidth;

	if (colIdx < 0) colIdx = 0;
	else if (colIdx >= m_NrOfCols) colIdx = m_NrOfCols - 1;


	return colIdx;
}

int CellSpace::PositionToRow(const Elite::Vector2 & pos) const
{
	int rowIdx = pos.y / m_CellHeight;

	if (rowIdx < 0) rowIdx = 0;
	else if (rowIdx >= m_NrOfRows) rowIdx = m_NrOfRows - 1;


	return rowIdx;
}