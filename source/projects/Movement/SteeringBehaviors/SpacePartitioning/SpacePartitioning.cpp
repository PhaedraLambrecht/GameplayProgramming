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


	// add cells
	for(int rowIdx{}; rowIdx < m_NrOfRows; ++rowIdx)
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
	m_Cells[PositionToIndex(pAgent->GetPosition())].agents.push_back(pAgent);
}

void CellSpace::UpdateAgentCell(SteeringAgent* pAgent, Elite::Vector2 oldPos)
{
	int oldCellIdx{ PositionToIndex(oldPos) };
	int newCellIdx{ PositionToIndex(pAgent->GetPosition()) };


	// Check if they aren't the same
	if( oldCellIdx != newCellIdx )
	{
		// delete pAgent from old cell
		m_Cells[oldCellIdx].agents.remove(pAgent);
		

		// add pAgent to new cell
		m_Cells[newCellIdx].agents.push_back(pAgent);
	}
	

}

void CellSpace::RegisterNeighbors(SteeringAgent* pAgent, float queryRadius)
{
	m_NrOfNeighbors = 0;// set neighbor count to 0


	// amount of cols and rows for neighborhood square
	int amountCols = ( queryRadius * 2) / m_CellWidth;
	int amountRows = ( queryRadius * 2) / m_CellHeight;

	// Bottom left coordinate of the binding box
	Elite::Vector2 leftbottomBindingBox
	{
		pAgent->GetPosition().x - queryRadius,
		pAgent->GetPosition().y - queryRadius
	};

	int cellsIdx{ PositionToIndex(leftbottomBindingBox) };// Get the index

	// Bottom left coordinate of the binding box
	Elite::Vector2 rightTopBindingBox
	{
		pAgent->GetPosition().x + queryRadius,
		pAgent->GetPosition().y + queryRadius
	};


	int maxRow{ PositionToRow(rightTopBindingBox) };
	int maxCol{ PositionToCol(rightTopBindingBox) };
	int minRow{ PositionToRow(leftbottomBindingBox) };
	int minCol{ PositionToCol(leftbottomBindingBox) };


	// Get the lowerBound
	int lowerBoundX{ int(IndexToColmRow(cellsIdx).x) };
	int lowerBoundY{ int(IndexToColmRow(cellsIdx).y) };



	for(int row{minRow}; row <= maxRow; ++row)
	{
		for (int col{ minCol }; col <= maxCol; ++col)
		{
			// rendering the active cells
			if (pAgent->CanRenderBehavior())
			{
				auto rectPoints{ m_Cells[row * amountCols + col].GetRectPoints() };

				DEBUGRENDERER2D->DrawPolygon(rectPoints.data(), rectPoints.size(), { 0.0f, 0.0f, 1.0f }, 0.0f);

			}


			// puts agents in the neighborhood vector
			for (const auto& foundAgent : m_Cells[row * amountCols + col].agents)
			{

				m_Neighbors[m_NrOfNeighbors] = pAgent;
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
	for(size_t idx{}; idx < m_Cells.size(); ++idx)
	{
		const std::vector <Elite::Vector2> cellRect{ m_Cells[idx].GetRectPoints() };

		// Draws the segments for each cell
		DEBUGRENDERER2D->DrawSegment(cellRect[0], cellRect[1], { 1.0f, 0.0f, 0.0f });
		DEBUGRENDERER2D->DrawSegment(cellRect[1], cellRect[2], { 1.0f, 0.0f, 0.0f });
		DEBUGRENDERER2D->DrawSegment(cellRect[2], cellRect[3], { 1.0f, 0.0f, 0.0f });
		DEBUGRENDERER2D->DrawSegment(cellRect[0], cellRect[3], { 1.0f, 0.0f, 0.0f });


		// TODO: add the cells agent num
			
		std::string agentCount{ std::to_string( m_Cells[idx].agents.size() ) };

		DEBUGRENDERER2D->DrawString({ cellRect[1].x, cellRect[1].y }, agentCount.c_str() );



		// TODO: show bounding box

			//float width{ m_Cells[idx].boundingBox.width};
			//float height{ m_Cells[idx].boundingBox.height };

			//Elite::Vector2 bototmLeft { m_Cells[idx].boundingBox.bottomLeft.x,
			//							m_Cells[idx].boundingBox.bottomLeft.y};

			//Elite::Vector2 topLeft{ bototmLeft.x, bototmLeft.y + height };
			//Elite::Vector2 botomRight{ bototmLeft.x + width, bototmLeft.y };
			//Elite::Vector2 topRight{ bototmLeft.x + width, bototmLeft.y + height };

			//DEBUGRENDERER2D->DrawSegment(bototmLeft, topLeft, { 0.0f, 0.0f, 1.0f });
			//DEBUGRENDERER2D->DrawSegment(topLeft, topRight, { 0.0f, 0.0f, 1.0f });
			//DEBUGRENDERER2D->DrawSegment(topRight, botomRight, { 0.0f, 0.0f, 1.0f });
			//DEBUGRENDERER2D->DrawSegment(bototmLeft, botomRight, { 0.0f, 0.0f, 1.0f });
		
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

Elite::Vector2 CellSpace::IndexToColmRow(const int index) const
{
	Elite::Vector2 colmAndRow{};

	colmAndRow.x = index / m_NrOfCols;
	colmAndRow.y = index % m_NrOfCols;


	// check if it is not under 0
	if (colmAndRow.x < 0)
	{
		colmAndRow.x = 0;
	}
	if (colmAndRow.y < 0)
	{
		colmAndRow.y = 0;
	}


	return  colmAndRow;

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