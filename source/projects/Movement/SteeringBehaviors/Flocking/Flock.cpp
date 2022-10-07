#include "stdafx.h"
#include "Flock.h"

#include "../SteeringAgent.h"
#include "../Steering/SteeringBehaviors.h"
#include "../CombinedSteering/CombinedSteeringBehaviors.h"

using namespace Elite;

//Constructor & Destructor
Flock::Flock(
	int flockSize /*= 50*/,
	float worldSize /*= 100.f*/,
	SteeringAgent* pAgentToEvade /*= nullptr*/,
	bool trimWorld /*= false*/)

	:m_WorldSize{ worldSize }
	, m_FlockSize{ flockSize }
	, m_TrimWorld{ trimWorld }
	, m_pAgentToEvade{ pAgentToEvade }
	, m_NeighborhoodRadius{ 15 }
	, m_NrOfNeighbors{ 0 }
{
	// Behaviors
	m_pSeekBehavior = new Seek();
	m_pSeparationBehavior = new Separation(this);
	m_pCohesionBehavior = new Cohesion(this);
	m_pVelMatchBehavior = new VelocityMatch(this);

	m_pWanderBehavior = new Wander();
	m_pEvadeBehavior = new Evade();


	// BlendedSteering
	m_pBlendedSteering = new BlendedSteering({ {m_pSeekBehavior, 0.5f},
																{m_pSeparationBehavior, 0.5f},
																{m_pCohesionBehavior, 0.5f},
																{m_pVelMatchBehavior, 0.5f},
																{m_pWanderBehavior, 0.5f} });
	// Priority steering
	m_pPrioritySteering = new PrioritySteering({ m_pEvadeBehavior, m_pBlendedSteering });


	// initialize the flock and the memory pool
	m_Agents.resize(m_FlockSize);
	m_Neighbors.resize(m_FlockSize);

	for (size_t idx{}; idx < m_Agents.size(); ++idx)
	{
		m_Agents[idx] = new SteeringAgent();
		m_Agents[idx]->SetMaxLinearSpeed(15.0f);
		m_Agents[idx]->SetSteeringBehavior(m_pPrioritySteering);
		m_Agents[idx]->SetAutoOrient(true);
		m_Agents[idx]->SetPosition(randomVector2(0, m_WorldSize));
		m_Agents[idx]->SetMass(0.3f);
	}


	// Initialize the AgentToEvade
	m_pAgentToEvade = new SteeringAgent();
	m_pAgentToEvade->SetSteeringBehavior(m_pSeekBehavior);
	m_pAgentToEvade->SetPosition(randomVector2(0, m_WorldSize));
	m_pAgentToEvade->SetMaxLinearSpeed(15.0f);
	m_pAgentToEvade->SetAutoOrient(true);
	m_pAgentToEvade->SetBodyColor({ 1.0f, 0.0f, 0.f, 1.0f });


	m_CanDebug = false;
};

Flock::~Flock()
{
	SAFE_DELETE(m_pSeekBehavior);
	SAFE_DELETE(m_pSeparationBehavior);
	SAFE_DELETE(m_pCohesionBehavior);
	SAFE_DELETE(m_pVelMatchBehavior);
	SAFE_DELETE(m_pWanderBehavior);
	SAFE_DELETE(m_pEvadeBehavior);

	SAFE_DELETE(m_pBlendedSteering);
	SAFE_DELETE(m_pPrioritySteering);


	SAFE_DELETE(m_pAgentToEvade);

	for(auto pAgent: m_Agents)
	{
		SAFE_DELETE(pAgent);
	}
	m_Agents.clear();
}

void Flock::Update(float deltaT)
{
	// Sets target to evade
	TargetData targetData{
							m_pAgentToEvade->GetPosition(),
				  m_pAgentToEvade->GetDirection().Normalize(),
							m_pAgentToEvade->GetLinearVelocity(),
							m_pAgentToEvade->GetAngularVelocity()
	};

	m_pEvadeBehavior->SetTarget(targetData);

	// Update the flock and AgentToEvade
	for(const auto agent: m_Agents)
	{
		// Neighbors
		RegisterNeighbors(agent);
		agent->Update(deltaT);

		// Updating the World Boundry
		if (m_TrimWorld)
		{
			agent->TrimToWorld(m_WorldSize);
		}
	}

	m_pAgentToEvade->Update(deltaT);

	// If needed trim the world size (AgentToEvade
	if(m_TrimWorld)
	{
		m_pAgentToEvade->TrimToWorld(m_WorldSize);
	}



	// Shows render behavior
	if(m_CanDebug)
	{
		for (const auto agent : m_Agents)
		{
			agent->SetRenderBehavior(true);
		}
	}
	else
	{
		for (const auto agent : m_Agents)
		{
			agent->SetRenderBehavior(false);
		}
		m_Agents[45]->SetRenderBehavior(true);
	}

	// Show neighborhood circle + change color to green if in it
	if (m_NeighborhoodDebug)
	{
		DEBUGRENDERER2D->DrawCircle(m_Agents[45]->GetPosition(), m_NeighborhoodRadius, { 1.0f, 1.0f, 1.0f, 1.0f }, 0.0f);

		// loop over all agents
		for (const auto agent : m_Agents)
		{
			// Calculate distance between agent[45] and 1 out of the flock
			float distance{ Distance(m_Agents[45]->GetPosition(), agent->GetPosition())};

			// check for self assignment
			if (agent != m_Agents[45])
			{
				// If  0 < distance < m_NeighborhoodRadius
				if ((distance > 0) && (distance < m_NeighborhoodRadius))
				{
					agent->SetBodyColor({ 0.0f, 1.0f, 0.0f, 1.0f });
				}
				else
				{
					agent->SetBodyColor({1.0f, 1.0f, 0.0f, 1.0f});
				}
			}
		}
		
	}


}

void Flock::Render(float deltaT)
{
	// Render flock and AgentToEvade
	for(SteeringAgent* pAgent: m_Agents)
	{
		pAgent->Render(deltaT);
	}

	m_pAgentToEvade->Render(deltaT);
}

void Flock::UpdateAndRenderUI()
{
	//Setup
	int menuWidth = 235;
	int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
	int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
	bool windowActive = true;
	ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
	ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
	ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::PushAllowKeyboardFocus(false);

	//Elements
	ImGui::Text("CONTROLS");
	ImGui::Indent();
	ImGui::Text("LMB: place target");
	ImGui::Text("RMB: move cam.");
	ImGui::Text("Scrollwheel: zoom cam.");
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("STATS");
	ImGui::Indent();
	ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Flocking");
	ImGui::Spacing();

	// TODO: Implement checkboxes for debug rendering and weight sliders here

	//checkbox
	ImGui::Checkbox("Trim World", &m_TrimWorld);
	ImGui::Checkbox("Debug render Steering", &m_CanDebug);
	ImGui::Checkbox("Debug render Neighborhood", &m_NeighborhoodDebug);
	


	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("Behavior Weights");
	ImGui::Spacing();


	//sliders to change the weight of the blended behavior
	ImGui::SliderFloat("Seek", &m_pBlendedSteering->GetWeightedBehaviorsRef()[0].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Separation", &m_pBlendedSteering->GetWeightedBehaviorsRef()[1].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Cohesion", &m_pBlendedSteering->GetWeightedBehaviorsRef()[2].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Alignment", &m_pBlendedSteering->GetWeightedBehaviorsRef()[3].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Wander", &m_pBlendedSteering->GetWeightedBehaviorsRef()[4].weight, 0.f, 1.f, "%.2");




	//End
	ImGui::PopAllowKeyboardFocus();
	ImGui::End();
	
}

void Flock::RegisterNeighbors(SteeringAgent* pAgent)
{
	m_NrOfNeighbors = 0;

	// loop over all agents
	for (const auto agent : m_Agents)
	{
		// Calculate distance between agent and 1 out of the flock
		float distance{ Distance(pAgent->GetPosition(), agent->GetPosition()) };

		// check for self assignment
		if(agent != pAgent)
		{
			// If  0 < distance < m_NeighborhoodRadius
			if((distance > 0) && (distance < m_NeighborhoodRadius))
			{
				m_Neighbors[m_NrOfNeighbors] = agent;
				++m_NrOfNeighbors;
			}
		}
	}
}

Elite::Vector2 Flock::GetAverageNeighborPos() const
{
	Vector2 avarage{};


	for (size_t idx{}; idx < m_NrOfNeighbors; ++idx)
	{
		avarage += m_Neighbors[idx]->GetPosition();
	}

	avarage /= m_NrOfNeighbors;


	return avarage;
}

Elite::Vector2 Flock::GetAverageNeighborVelocity() const
{
	// TODO: Implement - GetAverageNeighborVelocity

	Vector2 avarage{};


	for (size_t idx{}; idx < m_NrOfNeighbors; ++idx)
	{
		avarage += m_Neighbors[idx]->GetLinearVelocity();
	}

	avarage /= m_NrOfNeighbors;


	return avarage;
}

void Flock::SetTarget_Seek(TargetData target)
{
	m_pSeekBehavior->SetTarget(target);
}


float* Flock::GetWeight(ISteeringBehavior* pBehavior) 
{
	if (m_pBlendedSteering)
	{
		auto& weightedBehaviors = m_pBlendedSteering->GetWeightedBehaviorsRef();
		auto it = find_if(weightedBehaviors.begin(),
			weightedBehaviors.end(),
			[pBehavior](BlendedSteering::WeightedBehavior el)
			{
				return el.pBehavior == pBehavior;
			}
		);

		if(it!= weightedBehaviors.end())
			return &it->weight;
	}

	return nullptr;
}
