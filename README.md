# Gameplay Programming Research Topic - Flow Fields


![FlowFields_HowItWorks](https://user-images.githubusercontent.com/113975573/211923700-67323933-6597-4834-a07c-80c5d6882751.mp4)



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
![Planetary Annihilation](https://user-images.githubusercontent.com/113975573/211901126-ac0b321f-9dae-4519-b444-9591f3f746d0.png)


![FlowField_without_agents](https://user-images.githubusercontent.com/113975573/211901229-70d7b047-edc3-42ba-93c9-2ddc529733df.png)


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

**Agent management**

The agents have their own class. This class works together with the grid class to get the agents to move forward.
This is mainly shown in the MoveToNextSquare function (in grid).

Update in the agent class checks if the agent has reached the goal. If it has then the agent doesn’t move any longer and will be deleted after a certain amount of time.
If the agent has not reached the end than the next target gets set and the agent will update.
The movement of the agent is managed by the basic seek behaviour.

## Extra’s

**Placing obstacles**

If the left mouse button is pressed then there will be an obstacle placed on that square.
To be able to place the obstacles one should have the checkbox “obstacles ready” disabled.
Removing the obstacles is done in the same way as making them, click on the square with the left mouse button when “obstacles ready is disabled.

![Adding Obstacles and goals](https://user-images.githubusercontent.com/113975573/211924208-09c10c17-b281-406f-9bfe-6bc8e3f6de38.mp4)




**Placing agents**

When the goal and the obstacles are made only then can the agents be spawned at random position.
Once the first set of agents are gone you can spawn more agents.


## Future research

I definitely want to continue looking into this subject and potentially even add influence maps and working with those two to create a possible view of agents behaving like ants but having a set goal and a way to go there.
I would love to look into flow field with other algorithms.


![Showcasing the final result](https://user-images.githubusercontent.com/113975573/211924567-08292caf-be34-4811-9cf1-c64db51f7b61.mp4)




## References

1.	https://github.com/vonWolfehaus/flow-field

2.	https://leifnode.com/2013/12/flow-field-pathfinding/

3.	https://tyskwo.com/work/91_flow-field-pathfinding/

4.	https://www.youtube.com/watch?v=ZJZu3zLMYAc

5.	http://www.gameaipro.com/GameAIPro/GameAIPro_Chapter23_Crowd_Pathfinding_and_Steering_Using_Flow_Field_Tiles.pdfard

6.	Understanding Goal-Based Vector Field Pathfinding (tutsplus.com)

7.	https://github.com/GaelS/flowfield

8.	Planetary Annihilation: March 22nd LiveStream


