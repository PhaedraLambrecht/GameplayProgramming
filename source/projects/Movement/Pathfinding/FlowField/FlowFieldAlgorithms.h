#pragma once
#include "stdafx.h"
#include "Grid.h"


namespace Algorithms
{
	class FlowFieldAlgortithm
	{
	public:

		// Constructor and destructor
		FlowFieldAlgortithm(const Elite::Vector2* gridSize)
			:m_GridSize{ gridSize }
		{
		}
		virtual ~FlowFieldAlgortithm() = default;

		// Move and copy
		FlowFieldAlgortithm(const FlowFieldAlgortithm& flowfieldAlgorithm)				= delete;
		FlowFieldAlgortithm(FlowFieldAlgortithm&& flowfieldAlgorithm)					= delete;
		FlowFieldAlgortithm& operator=(const FlowFieldAlgortithm& flowfieldAlgorithm)	= delete;
		FlowFieldAlgortithm& operator=(FlowFieldAlgortithm&& flowfieldAlgorithm)		= delete;


		// Running the algorithm
		virtual void ActiveAlgorithm(int nrOfGoals, int goalIdx, std::vector<Grid::Square>* pGrid) = 0;

		// Actually making the flow field
		void FlowFieldCreation(int nrOfGoals, std::vector<Grid::Square>* pGrid, const std::vector<Elite::Vector2>& flowDirections)
		{
			int maxDiistance{ static_cast<int>(pGrid->size() + 1) }; // Max distance


			// For the neighbor validation/index
			int neighborColumn{};
			int neighborRow{};
			int neighborNodeIdx{};


			for (int nodeIdx{}; nodeIdx < int(pGrid->size()); ++nodeIdx)
			{
				// if the distance vector (if the idx exceeds the max size) is bigger, continue
				if (m_GridsDistance[nodeIdx] >= maxDiistance)
					continue;


				// Get the colmn/ row and the cheapest neighbor
				m_ColmnIdx = pGrid->at(nodeIdx).column;
				m_RowIdx = pGrid->at(nodeIdx).row;


				int closestDistance{ maxDiistance };
				int closestNeighborIdx{};


				for (int neighborIdx{}; neighborIdx < int(flowDirections.size()); ++neighborIdx)
				{

					neighborColumn = int(flowDirections[neighborIdx].x) + m_ColmnIdx;
					neighborRow = int(flowDirections[neighborIdx].y) + m_RowIdx;
					
					
					// Check if this "neighbor" is valid (i.e., within the grid)
					if (neighborColumn >= m_GridSize->x || neighborColumn < 0)
						continue;
					if( neighborRow >= m_GridSize->y	|| neighborRow < 0)
						continue;
					
			
				
					// Neighbors index in the grid
					neighborNodeIdx = neighborColumn + (neighborRow * int(m_GridSize->x) );
				
					// if the distance 
					if (m_GridsDistance[neighborNodeIdx] <= closestDistance)
					{
						closestDistance = m_GridsDistance[neighborNodeIdx];
						closestNeighborIdx = neighborIdx;
					}
				}
			

				// change the flow direction of this node to the flow direction calculated by it's neighbors
				pGrid->at(nodeIdx).flowDirections[nrOfGoals] = flowDirections[closestNeighborIdx];
			}
		}
		

	
	protected:

		struct nonVisitedIdx
		{
			int idx{};
			int distance{};
		};

		std::vector<int> m_GridsDistance{}; // Distances from the source node to each point in the flow field
		
		const Elite::Vector2* m_GridSize{}; // Colmn = x, row = y

		int m_ColmnIdx{};
		int m_RowIdx{};
	};


	// Dijkstra's algorithm
	class Dijkstra final :public FlowFieldAlgortithm
	{
	public:

		// Constructor and destructor
		Dijkstra(const Elite::Vector2* gridSize)
			:FlowFieldAlgortithm(gridSize)
		{
		};
		~Dijkstra() = default;


		// Move and copy
		Dijkstra(const Dijkstra& Dijkstra)				= delete;
		Dijkstra(Dijkstra&& Dijkstra)					= delete;
		Dijkstra& operator=(const Dijkstra& Dijkstra)	= delete;
		Dijkstra& operator=(Dijkstra&& Dijkstra)		= delete;



		// Running the algorithm
		void ActiveAlgorithm(int goalNr, int goalGridIdx, std::vector<Grid::Square>* pGrid) override
		{
			// Vector of indices to visit
			std::vector<nonVisitedIdx> nextToVisitIdxs{};
			
			// Max distance + 1, used for unexplored indexes
			int maxDistance{ static_cast<int>(pGrid->size()) + 1 };
			
		
			// Setting up dijkstra grid
			for (int idx{}; idx < int(pGrid->size()); ++idx)
			{
				if (pGrid->at(idx).squareType == Grid::SquareType::Obstacle)
				{
					//	Big int to avoid obstacles
					m_GridsDistance.push_back(100);
					continue;
				}
			
				m_GridsDistance.push_back(maxDistance);
			}
			
			
			// Goals = start position
			m_GridsDistance[goalGridIdx] = 0;
			
			
			// Used to calculate the neighbor of an index, no diagonals (x = column, y = row) 
			std::vector<Elite::Vector2> neighbors // TODO: research - look into doing diagonals 
			{	{ 1, 0 }, 
				{ -1, 0 }, 
				{ 0, 1 }, 
				{ 0, -1 } 
			};
			
			
			
			
			// Start point 
			// There are issues whem these aren't size_t
			size_t startColumn{};
			size_t startRow{};
			int neighborNodeIdx{};
			
			// Setting the initial index around the start node
			for (int idx{}; idx < 4; ++idx) // four because 4 around original node (no diagonals remember)
			{
				startColumn = pGrid->at(goalGridIdx).column + int(neighbors[idx].x);
				startRow = pGrid->at(goalGridIdx).row + int(neighbors[idx].y);
		
				// Checking it's bounds
				if (startColumn >= m_GridSize->x || startRow >= m_GridSize->y)
					continue;
			
			
				// Calculating the neighbors index
				neighborNodeIdx = startColumn + (startRow * int(m_GridSize->x));
			
			
				// if the square isn't an obstacle
				if (pGrid->at(neighborNodeIdx).squareType != Grid::SquareType::Obstacle)
				{
					// Set the distance to 1
					m_GridsDistance[neighborNodeIdx] = 1;
					// Add that square to the next to visit vector
					nextToVisitIdxs.push_back(nonVisitedIdx{ neighborNodeIdx, 1 });
				}
			}
			

			// Dijkstra's algorithm
			int toVisitColmn{};
			int toVisistRow{};
			
			Elite::Vector2 neighbor{};
			int neighborColmn{};
			int neighborRow{};
			
			
			// Loop over all the elements in the nextToVisitIdxs vector
			for (int Idx{}; Idx < int(nextToVisitIdxs.size()); ++Idx)
			{
				// Get the column and row of the current index
				
				toVisitColmn = pGrid->at(nextToVisitIdxs[Idx].idx).column;
				toVisistRow =  pGrid->at(nextToVisitIdxs[Idx].idx).row;
			
			
				// Check the neighbors for the current index
				for (int neighborIdx{}; neighborIdx < 4; ++neighborIdx)// four because 4 around original node (no diagonals remember) - yes this should be a variable by now
				{
					// Get the column and row of the current neighbor
					neighbor = neighbors[neighborIdx];
					neighborColmn = int(neighbor.x) + toVisitColmn;
					neighborRow = int(neighbor.y) + toVisistRow;
			
					/// Check if the neighbor is in the grid
					if (neighborColmn >= m_GridSize->x || neighborColmn < 0) 
						continue;
					if (neighborRow >= m_GridSize->y || neighborRow < 0) 
						continue;
					
			
			
					// Neighbors index in the grid
					int neighborNodeIdx{ neighborColmn + (neighborRow * int(m_GridSize->x)) };
			
			
					// Check if the neighbor has been explored
					if (m_GridsDistance[neighborNodeIdx] == maxDistance)
					{
						// Update the distance of the neighbor
						m_GridsDistance[neighborNodeIdx] = nextToVisitIdxs[Idx].distance + 1;
						// Add the neighbor to the to-visit list
						nextToVisitIdxs.push_back(nonVisitedIdx{ neighborNodeIdx, nextToVisitIdxs[Idx].distance + 1 });
					}
				}
			}
		}

		// Draw the costs
		// TODO: research - look more into this, get influence in here later as well
};



	// TODO: research - look into getting this with other algorithms
}

