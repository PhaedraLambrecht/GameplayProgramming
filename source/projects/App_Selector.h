#pragma once
// Here we determine which application is currently active
// Create the define here using the "ActiveApp_..." convention and set up the correct include and typedef in the #ifdef below.

//#define ActiveApp_Sandbox
//#define ActiveApp_Steering
//#define ActiveApp_CombinedSteering
//#define ActiveApp_Flocking
//#define ActiveApp_GraphTheory
//#define ActiveApp_AStar
//#define ActiveApp_NavMesh
//#define ActiveApp_FSM
//#define ActiveApp_BHT
//#define ActiveApp_InfluenceMap
//#define ActiveApp_AgarioInfluenceBHT

#define ActiveApp_FlowField



//---------- Registered Applications -----------
#ifdef ActiveApp_Sandbox
#include "projects/Movement/Sandbox/App_Sandbox.h"
typedef App_Sandbox CurrentApp;
#endif

#ifdef ActiveApp_Steering
#include "projects/Movement/SteeringBehaviors/Steering/App_SteeringBehaviors.h"
typedef App_SteeringBehaviors CurrentApp;
#endif // ActiveApp_Steering

#ifdef ActiveApp_CombinedSteering
#include "projects/Movement/SteeringBehaviors/CombinedSteering/App_CombinedSteering.h"
typedef App_CombinedSteering CurrentApp;
#endif // ActiveApp_CombinedSteering

#ifdef ActiveApp_Flocking
#include "projects/Movement/SteeringBehaviors/Flocking/App_Flocking.h"
typedef App_Flocking CurrentApp;
#endif // ActiveApp_Flocking

#ifdef ActiveApp_GraphTheory
#include "projects/Movement/Pathfinding/GraphTheory/App_GraphTheory.h"
typedef App_GraphTheory CurrentApp;
#endif // ActiveApp_GraphTheory

#ifdef ActiveApp_AStar
#include "projects/Movement/Pathfinding/AStar/App_PathfindingAStar/App_PathfindingAStar.h"
typedef App_PathfindingAStar CurrentApp;
#endif // ActiveApp_AStar

#ifdef ActiveApp_NavMesh
#include "projects/Movement/Pathfinding/NavMeshGraph/App_NavMeshGraph.h"
typedef App_NavMeshGraph CurrentApp;
#endif // ActiveApp_NavMesh

#ifdef ActiveApp_FSM
#include "projects/DecisionMaking/FiniteStateMachines/App_AgarioGame.h"
typedef App_AgarioGame CurrentApp;
#endif // ActiveApp_FSM

#ifdef ActiveApp_BHT
#include "projects/DecisionMaking/BehaviorTrees/App_AgarioGame_BT.h"
typedef App_AgarioGame_BT CurrentApp;
#endif // ActiveApp_BHT

#ifdef ActiveApp_InfluenceMap
#include "projects/DecisionMaking/InfluenceMaps/App_InfluenceMap.h"
typedef App_InfluenceMap CurrentApp;
#endif // ActiveApp_InfluenceMap

#ifdef ActiveApp_AgarioInfluenceBHT
#include "projects/DecisionMaking/InfluenceMaps/App_AgarioGame_InfluenceBT.h"
typedef App_AgarioGame_InfluenceBT CurrentApp;
#endif // ActiveApp_InfluenceMap

#ifdef ActiveApp_FlowField
#include "projects/Movement/Pathfinding/FlowField/App_FlowField.h"
typedef App_FlowField CurrentApp;
#endif // ActiveApp_FlowField

class App_Selector {
public:
	static IApp* CreateApp() {
		IApp* myApp =  new CurrentApp();
		return myApp;
	}
};