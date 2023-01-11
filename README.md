# Gameplay Programming Research Topic - Flow Fields



## Introduction

Pathfinding can be a big problem when using a lot of entities, using A* on a significantly high number of entities will become very expensive or you want the entities to keep the environments in mind when creating a path.
A possible solution for this is Flow Fields.

Flow fields (better known as vector field pathfinding) are grids of vectors, that are typically used to represent the flow of liquids. An example of this is environmental scientists using this way to see how water would flow.
They calculate the path from every node in the graph to the goal node at once, while looking at the cost of the terrain. This technique uses the Dijkstra algorithm.
The reality is that you can use a lot of different algorithms to get this result.

A few examples of the use case of flow fields are,
* Planet coaster: creating believable crowds.
* Physics: to look at fluid dynamics
* Planetary Annihilation: used for pathfinding of the units send into the world (example of cost field for tanks).



## Implementation

### General explanation

When the goal is set, the algorithm (in this case Dijkstra) gets called. It goes over the entire grid and sets the cost from node to node, starting from the goal.
Once this has finished it will set the direction to the neighbour which costs the least.
The agents will follow the directions of the node it is on to the goal.


### Grid and algorithm(s)

**Grid class**
In this class I create the grid and use it to create directions for the flow field.
First the directions get initialized, then I loop over the grid to find all the goals (only 1 goal possible).
For every goal index I call the algorithm and the flow field creation.

```
	//finding a goals
	std::vector<int> goalIndxs{};

	for (const auto& gridSqr : *m_pGrid)
	{
		if (gridSqr.squareType == Grid::SquareType::Goal)
			goalIndxs.push_back(gridSqr.column + (gridSqr.row * m_GridSize.x));
	}


	Algorithms::Dijkstra* dijkstraAlgorithm = new Algorithms::Dijkstra(&m_GridSize);



	//for every goal: run algorithm and make flowfield;
	for (int idx{}; idx < int(goalIndxs.size()); ++idx)
	{
		dijkstraAlgorithm->ActiveAlgorithm(idx, goalIndxs[idx], m_pGrid);
		dijkstraAlgorithm->FlowFieldCreation(idx, m_pGrid, flowfieldFlowDirections);
	}
```

**Algorithm(s): Dijkstra**
First I set up the costs to be able to use the Dijkstra algorithm.
Iterating over the entire grid and checking if it is an obstacle or not, when it is an obstacle I set the cost too 100. Otherwise I set the cost to the max cost (which was calculated earlier).

```
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
```

The beginning point is set by going over the neighbours of the goal. Then I check if the square is withing the grid and if it isn’t an obstacle. When these conditions are met the cost gets set to 1.

```
for (int idx{}; idx < 4; ++idx) // four because 4 around original node
{
    startColumn = pGrid->at(goalGridIdx).column + int(neighbors[idx].x);
    startRow = pGrid->at(goalGridIdx).row + int(neighbors[idx].y);

    // Checking it's bounds
    if (startColumn >= m_GridSize->x || startRow >= m_GridSize->y)
    {
    continue;
    }


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
```

Short Explanation of Dijkstra’s algorithm
1.	Go over all the next squares that have to be visited.
  -	Go over all the neighbours.
    -	Check if the neighbour has been visited.
      -	The cost will be set to cost + 1.
        -	Add this neighbour to the vector of next to visit squares.










