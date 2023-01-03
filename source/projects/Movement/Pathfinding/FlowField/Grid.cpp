//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "Grid.h"


Grid::Grid(const Elite::Vector2& worldSize, const Elite::Vector2& gridResolution)
	:m_WorldDimensions{ worldSize }
	,m_ColmnRow(gridResolution)
	,m_SquareSize{ worldSize.x / gridResolution.x, worldSize.y / gridResolution.y }
{
	InitializeGrid();
}
Grid::~Grid()
{
	SAFE_DELETE(m_pGrid);
}


void Grid::Render(float deltaTime) const
{
	// Draws the grid in it's total
	if (m_DrawGrid) 
		DrawGrid();

	// Draws each obsticals
	if (m_DrawObstacles) 
		DrawObstacles();

	// Draws each goal
	if (m_DrawGoals)
		DrawGoals();

	// Draws the direction of each square
	//if (m_DrawDirections)
	//	DrawDirections();
}

void Grid::Update(float deltaTime)
{
}


void Grid::AddObstacle(const Elite::Vector2& obstaclePos)
{
	// Get the index of the position
	int squareIdx{ GetSquareIdxAtPos(obstaclePos) };
	// Get the type of that square
	Grid::SquareType& squareType{ m_pGrid->at(squareIdx).squareType };


	if (squareType != SquareType::Obstacle)
	{
		squareType = SquareType::Obstacle;
	}
	else
	{
		squareType = SquareType::Default;
	}

}

void Grid::AddGoal(const Elite::Vector2& goalPos)
{
	// Get the index of the position
	int index{ GetSquareIdxAtPos(goalPos) };
	// Get the type of that square
	Grid::SquareType& sqrType{ m_pGrid->at(index).squareType };


	if (sqrType != SquareType::Goal)
	{
		sqrType = SquareType::Goal;
	}
	else
	{ 
		sqrType = SquareType::Default;
	}
}
void Grid::MakeGoalVector()
{
	m_MadeGoalVector = true;
	
	// Check every square
	for (int idx{}; idx < m_pGrid->size(); ++idx)
	{	
		// If it is a goal, push the middle on the vector
		if (m_pGrid->at(idx).squareType == SquareType::Goal)
		{
			m_Goals.push_back(GetMidOfSquare(idx));
		}
	}
}



//------------------
// Helper functins
//------------------
void Grid::InitializeGrid()
{
	// Initialze the grid
	m_pGrid = new std::vector<GridSquare>{};

	// Initialize the world bounds
	const float worldBottom{ -m_WorldDimensions.y / 2 };
	const float worldTop{ m_WorldDimensions.y / 2 };
	const float worldLeft{ -m_WorldDimensions.x / 2 };
	const float worldRight{ m_WorldDimensions.x / 2 };



	// Don't want these to be ramade every loop, only overwriten
	Elite::Vector2 BottomLeftPos{};
	GridSquare square{};

	for (int rowIdx{}; rowIdx < m_ColmnRow.y; ++rowIdx)// Loop over all the rows
	{
		// Calculate the bottom
		BottomLeftPos.y = worldBottom + (m_SquareSize.y * rowIdx) * (worldTop - worldBottom) / m_WorldDimensions.y;
		
		for (int columnIdx{}; columnIdx < m_ColmnRow.x; ++columnIdx)// Loop over all the colmns
		{
			// Calculate the left
			BottomLeftPos.x = worldLeft + (m_SquareSize.x * columnIdx) * (worldRight - worldLeft) / m_WorldDimensions.x;
			
			square.row = rowIdx;
			square.column = columnIdx;
			square.bottomLeft = BottomLeftPos;
			

			// push it on the grid vector
			m_pGrid->push_back(square);
		}
	}
}

void Grid::DrawGridSquare(int idx, const Elite::Color& color, bool fillSqr) const
{
	// Make a vector of points
	Elite::Vector2 squarePoint[4]{};
	const auto& square = m_pGrid->at(idx);

	// Initialize those points
	squarePoint[0] = square.bottomLeft; // Bototm left
	squarePoint[1] = Elite::Vector2{ square.bottomLeft.x + m_SquareSize.x, square.bottomLeft.y }; // Bottom rigth
	squarePoint[2] = Elite::Vector2{ square.bottomLeft.x + m_SquareSize.x, square.bottomLeft.y + m_SquareSize.y }; // Top right
	squarePoint[3] = Elite::Vector2{ square.bottomLeft.x, square.bottomLeft.y + m_SquareSize.y }; // Top left


	if (fillSqr) // only when not default
	{
		DEBUGRENDERER2D->DrawSolidPolygon(&squarePoint[0], 4, color, DEBUGRENDERER2D->NextDepthSlice());
	}
	else
	{
		DEBUGRENDERER2D->DrawPolygon(&squarePoint[0], 4, color, DEBUGRENDERER2D->NextDepthSlice());
	}
}
void Grid::DrawGrid() const
{	
	// Check every square
	for (int idx{}; idx < m_pGrid->size(); ++idx)
	{
		// If it isn't a default, stop the loop
		if (m_pGrid->at(idx).squareType != SquareType::Default)
			continue;

		// Else, draw the square (red)
		DrawGridSquare(idx, m_GridColor, false);
	}
}
void Grid::DrawObstacles() const
{
	// Check every square
	for (int idx{}; idx < m_pGrid->size(); ++idx)
	{
		// If it isn't an obstacle, stop the loop
		if (m_pGrid->at(idx).squareType != SquareType::Obstacle) 
			continue;

		// Else, draw the square (blue)
		DrawGridSquare(idx, m_ObstacleColor, true);
	}
}
void Grid::DrawGoals() const
{
	// Check every square
	for (int idx{}; idx < m_pGrid->size(); ++idx)
	{
		// If it isn't a goal, stop the loop
		if (m_pGrid->at(idx).squareType != SquareType::Goal)
			continue;

		// Else, draw the square (green filiing)
		DrawGridSquare(idx, m_GoalColor, true);
	}
}

void Grid::DrawDirections() const
{
	int directionNr{ 0 };
	float directionLength{ 4.f };
	
	// Go over every square
	for (int idx{}; idx < m_pGrid->size(); ++idx)
	{
		// If it isn't default, stop the loop
		if (m_pGrid->at(idx).squareType != SquareType::Default)
			continue;

		// Else, draw the direction
		DEBUGRENDERER2D->DrawDirection(GetMidOfSquare(idx), m_pGrid->at(idx).flowDirections[directionNr], directionLength, m_DirectionColor);
		DEBUGRENDERER2D->DrawSolidCircle(Elite::Vector2{ GetMidOfSquare(idx).x + directionLength, GetMidOfSquare(idx).y + (directionLength / 2.0f) }, 0.5f, Elite::Vector2{0.0f, 0.0f}, Elite::Color{1.0f, 1.0f, 1.0f});
		DEBUGRENDERER2D->DrawPoint(GetMidOfSquare(idx), 5.f, { 0, 0, 0 });
	}
}

int Grid::GetSquareIdxAtPos(const Elite::Vector2& pos) const
{
	for (int rowIdx{}; rowIdx < m_ColmnRow.y; ++rowIdx) // Going over all the rows
	{
		for (int ColumnIdx{}; ColumnIdx < m_ColmnRow.x; ++ColumnIdx)// Going over all the colmns
		{
			int index{ ColumnIdx + (rowIdx * (int)m_ColmnRow.x) };
			// Getting the bottom left of that square
			Elite::Vector2 BottomLeft{ m_pGrid->at(index).bottomLeft };// don't get why it needs to be at()
	
			// Is the position withing the square ?
			if (pos.x >= BottomLeft.x && 
				pos.x <= (BottomLeft.x + m_SquareSize.x) &&
				pos.y >= BottomLeft.y &&
				pos.y <= (BottomLeft.y + m_SquareSize.y)
				)
			{
				return index;
			}
		}
	}
	
	return 0;
}
