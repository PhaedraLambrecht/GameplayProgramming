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

