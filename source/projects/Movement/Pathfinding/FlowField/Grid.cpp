//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "FlowFieldAlgorithms.h"
#include "Grid.h"


Grid::Grid(const Elite::Vector2& worldSize, const Elite::Vector2& gridSize)
	:m_WorldDimensions{ worldSize }
	,m_GridSize(gridSize)
	,m_SquareSize{ worldSize.x / gridSize.x, worldSize.y / gridSize.y }
{
	InitializeGrid();
}

Grid::~Grid()
{
	SAFE_DELETE(m_pGrid);
}


// Funcitons Specifically for the agents
bool Grid::MoveToNextSquare(const Elite::Vector2& currentPos, Elite::Vector2& targetPos, bool firstMove)
{
	const float distance{ Elite::Distance(currentPos, targetPos) };

	// If this is the first move, or the distance between the current position and target position is less than or equal to 2, we need to find a new target position
	if (firstMove || (distance <= 2.0f) )
	{

		// Get the index of the square the agent is currently in
		const int CurrentSquareIdx = GetIdxAtPos(currentPos);


		// Calculate a position
		const Elite::Vector2 normalizedFlowDirection{ m_pGrid->at(CurrentSquareIdx).flowDirections[0].GetNormalized() };
		const float length{ m_SquareSize.x + (m_SquareSize.x / 2) };
		
		//from the current position of the agent along the direction the agent should be following over a length ( 1.5 times a square's length)
		const Elite::Vector2 nextSqrPosFromDirection{ currentPos + (normalizedFlowDirection * length) };


		// Get the index of the square at this new position
		const int newIdx{ GetIdxAtPos(nextSqrPosFromDirection) };


		// Set the next target position for the agent to the center of this new square
		targetPos = GetSquareCenter(newIdx);


		return true;
	}


	return false;
}

Elite::Vector2 Grid::GetRandomPos()
{
	// TODO: research - look more into cuz wtf + want them to spawn at mouseclick
	int randomIdx{};

	do
	{
		randomIdx = Elite::randomInt(m_pGrid->size() - 1);
	} 
	while (m_pGrid->at(randomIdx).squareType != SquareType::Default);


	return GetSquareCenter(randomIdx);;
}

bool Grid::AgentReachedGoal(const Elite::Vector2& agentPos)
{
	bool isAtGoal{ GetIdxAtPos(agentPos) == GetIdxAtPos(m_goal) };

	return  isAtGoal;
}



// Functions
void Grid::Render(float deltaTime) const
{
	// Draws each cell in the grid
	DrawGrid();

	// Draws the direction of each square
	if (m_DrawDirections)
		DrawDirections();
}

void Grid::Update(float deltaTime)
{
	if (m_MadeGoal && !m_MadeFlowFields)
	{
		m_MadeFlowFields = true;
		MakeFlowfield();
		m_MadeGoal = false;
	}
}


void Grid::AddObstacle(const Elite::Vector2& obstaclePos)
{
	// Get the index of the position
	int squareIdx{ GetIdxAtPos(obstaclePos) };
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
	int index{ GetIdxAtPos(goalPos) };
	// Get the type of that square
	Grid::SquareType& sqrType{ m_pGrid->at(index).squareType };



		if (sqrType != SquareType::Goal)
		{
			if (m_CurrentGoalCount != m_MaxGoals)
			{
				sqrType = SquareType::Goal;
				++m_CurrentGoalCount;
			}
		}
		else
		{ 
			sqrType = SquareType::Default;
			--m_CurrentGoalCount;
		}
}


void Grid::MakeFlowfield()
{
	// Every square around a square
	const std::vector<Elite::Vector2> flowfieldFlowDirections
	{	{ 1, 0 }, 
		{ 1, 1 }, 
		{ 0, 1 }, 
		{ -1, 1 }, 
		{ -1, 0 }, 
		{ -1, -1 },
		{ 0, -1 }, 
		{1,-1} 
	}; 


	//finding a goals
	std::vector<int> goalIndxs{};

	for (const auto& gridSqr : *m_pGrid)
	{
		if (gridSqr.squareType == Grid::SquareType::Goal)
			goalIndxs.push_back(gridSqr.column + (gridSqr.row * m_GridSize.x));
	}


	Algorithms::Dijkstra* dijkstraAlgorithm = new Algorithms::Dijkstra(&m_GridSize);


	
	//for every goal: run algorithm and make flowfield;
	for (int idx{}; idx < goalIndxs.size(); ++idx)
	{
		dijkstraAlgorithm->ActiveAlgorithm(idx, goalIndxs[idx], m_pGrid);
		dijkstraAlgorithm->FlowFieldCreation(idx, m_pGrid, flowfieldFlowDirections);
	}

	delete dijkstraAlgorithm;
	m_MadeFlowFields = false;

}

void Grid::CreateGoal()
{
	m_MadeGoal = true;

	// Go over all the squares in the grid
	for (int idx{}; idx < m_pGrid->size(); ++idx)
	{
		if (m_pGrid->at(idx).squareType == SquareType::Goal)
		{
			m_pGrid->at(idx).flowDirections.push_back({ 0, 0 });

			m_goal = GetSquareCenter(idx);
		}
	}
}

//int Grid::GetNewGoal(int currentGoal) const
//{
//	int newGoalIdx{};
//	do
//	{
//		newGoalIdx = Elite::randomInt(m_Goals.size() - 1);
//	} 
//	while (newGoalIdx == currentGoal);
//	
//	
//	return newGoalIdx;
//}

//------------------
// Helper functins
//------------------
void Grid::InitializeGrid()
{
	// Initialze the grid
	m_pGrid = new std::vector<Square>{};

	// Initialize the world bounds
	const float worldBottom{ -m_WorldDimensions.y / 2 };
	const float worldTop{ m_WorldDimensions.y / 2 };
	const float worldLeft{ -m_WorldDimensions.x / 2 };
	const float worldRight{ m_WorldDimensions.x / 2 };



	// Don't want these to be ramade every loop, only overwriten
	Elite::Vector2 BottomLeftPos{};
	Square square{};

	for (int rowIdx{}; rowIdx < m_GridSize.y; ++rowIdx)// Loop over all the rows
	{
		// Calculate the bottom
		BottomLeftPos.y = worldBottom + (m_SquareSize.y * rowIdx) * (worldTop - worldBottom) / m_WorldDimensions.y;
		
		for (int columnIdx{}; columnIdx < m_GridSize.x; ++columnIdx)// Loop over all the colmns
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
	Elite::Vector2 squarePoint[4]{};// A square always has 4 points.... i hope
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
		//TODO: reseearch - look into making this switch

		if (m_pGrid->at(idx).squareType == SquareType::Obstacle)// If it is an obstacle
		{
			if (m_DrawObstacles)
			{
				// Draw the square (blue)
				DrawGridSquare(idx, m_ObstacleColor, true);
			}
		}
		else if(m_pGrid->at(idx).squareType == SquareType::Goal)// If it is a goal
		{
			if (m_DrawGoals)
			{
				// Draw the square (green filiing)
				DrawGridSquare(idx, m_GoalColor, true);
			}
		}
		else // If it is a normal square
		{
			if (m_DrawGrid)
			{
				//draw the square (red)
				DrawGridSquare(idx, m_GridColor, false);
			}
		}
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
		DEBUGRENDERER2D->DrawDirection(GetSquareCenter(idx), m_pGrid->at(idx).flowDirections[directionNr].GetNormalized(), directionLength, m_DirectionColor);
	//	DEBUGRENDERER2D->DrawSolidCircle(Elite::Vector2{ GetMidOfSquare(idx).x + directionLength, GetMidOfSquare(idx).y + (directionLength / 2.0f) }, 0.5f, Elite::Vector2{0.0f, 0.0f}, Elite::Color{1.0f, 1.0f, 1.0f});
		DEBUGRENDERER2D->DrawPoint(GetSquareCenter(idx), 5.f, { 0, 0, 0 });
	}
}


int Grid::GetIdxAtPos(const Elite::Vector2& pos) const
{
	for (int rowIdx{}; rowIdx < m_GridSize.y; ++rowIdx) // Going over all the rows
	{
		for (int ColumnIdx{}; ColumnIdx < m_GridSize.x; ++ColumnIdx)// Going over all the colmns
		{

			int index{ ColumnIdx + (rowIdx * (int)m_GridSize.x) };
			// Getting the bottom left of that square
			Elite::Vector2 BottomLeft{ m_pGrid->at(index).bottomLeft };
	

			// If the position is within the square, return the index
			if (pos.x >= BottomLeft.x						&& 
				pos.x <= (BottomLeft.x + m_SquareSize.x)	&&
				pos.y >= BottomLeft.y						&&
				pos.y <= (BottomLeft.y + m_SquareSize.y)
			   )
			{
				return index;
			}

		}
	}
	
	// Return 0 to not get an out of bounds problem
	return 0;
}
