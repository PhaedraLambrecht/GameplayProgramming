#pragma once


class Grid
{
public:


	enum class SquareType
	{
		Default, Obstacle, Goal
	};

	struct Square
	{
		int row{};
		int	column{};
		Elite::Vector2 bottomLeft{};

		std::vector<Elite::Vector2> flowDirections{ {0.0f, 0.0f} };
		SquareType squareType{ SquareType::Default };
	};




	//Constructor & Destructor
	Grid(const Elite::Vector2& worldSize, const Elite::Vector2& gridResolution);
	~Grid();

	// Move and copy
	Grid(const Grid& Grid)				= delete;
	Grid(Grid&& Grid)					= delete;
	Grid& operator=(const Grid& Grid)	= delete;
	Grid& operator=(Grid&& Grid)		= delete;


	// Functions
	void Render(float deltaTime) const;
	void Update(float deltaTime);


	bool MoveSqaure(const Elite::Vector2& currentPos, Elite::Vector2& targetPos, int goalNr, bool firstMove);
	Elite::Vector2 GetValidRandomPos();
	void MakeFlowfield();


	// Adding obstacles/goals(s)
	void AddObstacle(const Elite::Vector2& obstaclePos);
	void AddGoal(const Elite::Vector2& goalPos);


	//goal vector
	void CreateGoalVector();
	bool GoalVectorReady() { return m_MadeGoalVector; };
	int GetNewGoal(int currentGoal) const;

	// Toggles for visuals
	void ToggleDrawGrid(bool canDraw) { m_DrawGrid = canDraw; };
	void ToggleDrawObstacles(bool canDraw) { m_DrawObstacles = canDraw; };
	void ToggleDrawGoals(bool canDraw) { m_DrawGoals = canDraw; };
	void ToggleDrawDirections(bool canDraw) { m_DrawDirections = canDraw; };


private:

	std::vector<Square>* m_pGrid{ nullptr };


	// Square/grid helpers
	Elite::Vector2 m_WorldDimensions;
	Elite::Vector2 m_GridSize; // colmn = x, row = y
	Elite::Vector2 m_SquareSize;

	
	// Square colors
	Elite::Color m_GridColor{ 1.f, 0.f, 0.f };		// Red
	Elite::Color m_GoalColor{ 0.f, 1.f, 0.f };		// Green
	Elite::Color m_ObstacleColor{ 0.f, 0.f, 1.f };	// Bleu
	Elite::Color m_DirectionColor{ 1.f, 1.f, 1.f };	// White


	// Visualization bools
	bool m_DrawGrid{ true };
	bool m_DrawObstacles{ true };
	bool m_DrawGoals{ true };
	bool m_DrawDirections{ true };


	bool madeFlowFields{ false };

	bool m_MadeGoalVector{ false };
	std::vector<Elite::Vector2> m_Goals{};


	float m_MindDistanceFromTarget{ 2.f };


	//-------------------
	// Helper functions
	//-------------------
	void InitializeGrid();

	//grid drawing
	void DrawGridSquare(int idx, const Elite::Color& color, bool fillSqr) const;
	void DrawGrid() const;
	void DrawDirections() const;
	void DrawObstacles() const;
	void DrawGoals() const;

	//get the mid position of a square
	Elite::Vector2 GetMidOfSquare(size_t idx) const
	{
		return m_pGrid->at(idx).bottomLeft + (m_SquareSize / 2.f);
	};
	int GetSquareIdxAtPos(const Elite::Vector2& pos) const;
};

