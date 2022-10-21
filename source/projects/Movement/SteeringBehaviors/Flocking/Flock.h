#pragma once
#include "../SteeringHelpers.h"
#include "FlockingSteeringBehaviors.h"
#include "projects/Movement/SteeringBehaviors/SpacePartitioning/SpacePartitioning.h"

class ISteeringBehavior;
class SteeringAgent;
class BlendedSteering;
class PrioritySteering;
class CellSpace;


class Flock final
{
public:
	Flock(
		int flockSize = 50, 
		float worldSize = 100.f, 
		SteeringAgent* pAgentToEvade = nullptr, 
		bool trimWorld = false);

	~Flock();



	void Update(float deltaT);
	void UpdateAndRenderUI() ;
	void Render(float deltaT);

	void RegisterNeighbors(SteeringAgent* pAgent);
	int GetNrOfNeighbors() const { return m_NrOfNeighbors; }
	const std::vector<SteeringAgent*>& GetNeighbors() const { return m_Neighbors; }

	Elite::Vector2 GetAverageNeighborPos() const;
	Elite::Vector2 GetAverageNeighborVelocity() const;

	void SetTarget_Seek(TargetData target);
	void SetWorldTrimSize(float size) { m_WorldSize = size; }

private:

	// Initialising consts
	const float m_DefaultSeekWeight;
	const float m_DefaultSeparationWeight;
	const float m_DefaultCohesionWeight;
	const float m_DefaultAlignmentWeight;
	const float m_DefaultWanderWeight;


	// Datamembers
	int m_FlockSize = 0;
	std::vector<SteeringAgent*> m_Agents;
	std::vector<SteeringAgent*> m_Neighbors;
	std::vector<Elite::Vector2> m_AgentOldPos;


	bool m_TrimWorld = false;
	float m_WorldSize = 0.f;
	bool m_CanDebug;
	bool m_CanNeighborhoodDebug;
	bool m_CanSpatialPartitioning;


	float m_NeighborhoodRadius = 10.f;
	int m_NrOfNeighbors = 0;

	SteeringAgent* m_pAgentToEvade = nullptr;


	// Steering Behaviors
	Seek* m_pSeekBehavior = nullptr;
	Separation* m_pSeparationBehavior = nullptr;
	Cohesion* m_pCohesionBehavior = nullptr;
	Alignment* m_pAlignmentBehavior = nullptr;
	Wander* m_pWanderBehavior = nullptr;
	Evade* m_pEvadeBehavior = nullptr;

	BlendedSteering* m_pBlendedSteering = nullptr;
	PrioritySteering* m_pPrioritySteering = nullptr;


	// Cells space
	CellSpace* m_pCellSpace;


	// Functions
	float* GetWeight(ISteeringBehavior* pBehaviour);


	// Initializer
	void InitializeAgents();
	void InitializeBehaviours();


	// Updates
	void UpdateAgentToEvade();
	void UpdateCellSpace(int index, float deltaT);
	void UpdateDebugRendering(int index);


	// Render helpers
	void RenderNeighbourhood(float deltaT);


	// UI
	void SetupUI();
	void DisplayUI();
	void DisplayCheckBoxes();
	void DisplaySliders();

private:

	Flock(const Flock& other);
	Flock& operator=(const Flock& other);
};