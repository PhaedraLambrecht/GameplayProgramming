#pragma once
#include "../../SteeringBehaviors/SteeringAgent.h"
#include "../../SteeringBehaviors/Steering/SteeringBehaviors.h"


class FlowAgent final
{
public:

	//Constructor & Destructor
	FlowAgent(const Elite::Vector2& spawnPos);
	~FlowAgent();

	// Move and copy
	FlowAgent(const FlowAgent& FlowAgent)				= delete;
	FlowAgent(FlowAgent&& FlowAgent)					= delete;
	FlowAgent& operator=(const FlowAgent& FlowAgent)	= delete;
	FlowAgent& operator=(FlowAgent&& FlowAgent)			= delete;




	// Functions
	void Update(float dt);
	void Render(float dt);

	//void TrimToWorld(float worldBounds, bool isWorldLooping);


	// Needed for movement
	Elite::Vector2 GetPos() const;
	Elite::Vector2& GetTargetPos();

	void SetHasReachedGoal(bool reachedGoal);
	bool GetHasReachedGoal() const;
	bool GetFirstMove() const;


private:

	SteeringAgent* m_pAgent{};
	Elite::Color m_AgentColor{ 0.5f, 0.5f, 0.0f };// They are now ugly mustard colored =)

	
	Seek* m_pBehavior{};
	Elite::Vector2 m_TargetPos{ 0.f,0.f };


	bool m_HasReachedGoal{ false };
	bool m_HasMadeFirstMove{ true };
	

	//-------------------
	// Helper functions
	//-------------------
	void DrawTarget() const;
	//void TrimToWorld(const Elite::Vector2& bottomLeft, const Elite::Vector2& topRight, bool isWorldLooping = true) const;
};
