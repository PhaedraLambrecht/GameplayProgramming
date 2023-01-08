#pragma once
#include "../../SteeringBehaviors/SteeringAgent.h"



class FlowAgent final : SteeringAgent
{
public:

	//Constructor & Destructor
	FlowAgent(const Elite::Vector2& spawnPos);
	~FlowAgent();



	// Move and copy
	FlowAgent(const FlowAgent& FlowAgent) = delete;
	FlowAgent(FlowAgent&& FlowAgent) = delete;
	FlowAgent& operator=(const FlowAgent& FlowAgent) = delete;
	FlowAgent& operator=(FlowAgent&& FlowAgent) = delete;


	// Functions
	void Update(float dt) override;
	void Render(float dt) override;


private:

	Elite::Color m_AgentColor{ 1.f, 1.f, 1.f };

	SteeringAgent* m_pAgent{};
};

