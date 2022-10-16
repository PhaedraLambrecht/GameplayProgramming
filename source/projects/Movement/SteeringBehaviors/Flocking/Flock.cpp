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
	m_pAlignmentBehavior = new Alignment(this);

	m_pWanderBehavior = new Wander();
	m_pEvadeBehavior = new Evade();


	// BlendedSteering
	std::vector<BlendedSteering::WeightedBehavior> weightedSteeringBehaviors;
	weightedSteeringBehaviors.push_back({ m_pSeekBehavior, 0.2f });
	weightedSteeringBehaviors.push_back({ m_pSeparationBehavior, 0.4f });
	weightedSteeringBehaviors.push_back({ m_pCohesionBehavior, 0.2f });
	weightedSteeringBehaviors.push_back({ m_pAlignmentBehavior, 0.4f });
	weightedSteeringBehaviors.push_back({ m_pWanderBehavior, 0.4f });
	m_pBlendedSteering = new BlendedSteering(weightedSteeringBehaviors);


	// Priority steering
	m_pPrioritySteering = new PrioritySteering({ m_pEvadeBehavior, m_pBlendedSteering });

	// initialize the partitioning
	m_CellSpace = new CellSpace(m_WorldSize, m_WorldSize, 25, 25, m_FlockSize);


	// initialize the memory pool
	m_Agents.resize(m_FlockSize);
	m_Neighbors.resize(m_FlockSize);
	m_AgentOldPos.resize(m_FlockSize);


	// Initializing the flock
	for (size_t idx{}; idx < m_Agents.size(); ++idx)
	{
		m_Agents[idx] = new SteeringAgent();
		m_Agents[idx]->SetMaxLinearSpeed(55.0f);
		m_Agents[idx]->SetMaxAngularSpeed(25.0f);
		m_Agents[idx]->SetSteeringBehavior(m_pPrioritySteering);
		m_Agents[idx]->SetAutoOrient(true);
		m_Agents[idx]->SetPosition(randomVector2(0, m_WorldSize));
		m_Agents[idx]->SetMass(1.0f);

		// Add the agents to the space partitioning
		m_CellSpace->AddAgent(m_Agents[idx]);

		m_AgentOldPos[idx] = m_Agents[idx]->GetPosition();
	}



	// initialize the bools
	m_CanDebug = false;
	m_NeighborhoodDebug = false;
	m_SpatialPartitioning = true;

};

Flock::~Flock()
{
	SAFE_DELETE(m_pSeekBehavior);
	SAFE_DELETE(m_pSeparationBehavior);
	SAFE_DELETE(m_pCohesionBehavior);
	SAFE_DELETE(m_pAlignmentBehavior);
	SAFE_DELETE(m_pWanderBehavior);
	SAFE_DELETE(m_pEvadeBehavior);

	SAFE_DELETE(m_pBlendedSteering);
	SAFE_DELETE(m_pPrioritySteering);


	for(auto pAgent: m_Agents)
	{
		SAFE_DELETE(pAgent);
	}
	m_Agents.clear();


	SAFE_DELETE(m_CellSpace);
}

void Flock::Update(float deltaT)
{
	if(m_pAgentToEvade != nullptr)
	{
		m_pEvadeBehavior->SetTarget(m_pAgentToEvade->GetPosition() );
	}


	// Update the flock and AgentToEvade
	for (int idx{}; idx < m_Agents.size(); ++idx)
	{
		// If Spatial partitioning is used
		if(m_SpatialPartitioning)
		{
			m_CellSpace->RegisterNeighbors(m_Agents[idx], m_NeighborhoodRadius);
			m_Agents[idx]->Update(deltaT);
			m_CellSpace->UpdateAgentCell(m_Agents[idx], m_AgentOldPos[idx]);

			m_AgentOldPos[idx] = m_Agents[idx]->GetPosition();
		}
		// If Spatial Partitioning is not used
		else
		{
			RegisterNeighbors(m_Agents[idx]);
			m_Agents[idx]->Update(deltaT);
		}



		// Updating the World Boundry
		if (m_TrimWorld)
		{
			m_Agents[idx]->TrimToWorld(m_WorldSize);
		}
	}
}

void Flock::Render(float deltaT)
{
	//if(m_SpatialPartitioning)
	//{
	//	m_CellSpace->RenderCells();
	//}
	//
	//
	//// Render flock and AgentToEvade
	//for(SteeringAgent* pAgent: m_Agents)
	//{
	//	pAgent->Render(deltaT);
	//}
	//
	//
	//m_pAgentToEvade->Render(deltaT);


	for (int idx{}; idx < m_Agents.size(); ++idx)
	{
		// Shows render behavior
		if (m_CanDebug)
		{
			m_Agents[idx]->SetRenderBehavior(true);
		}
		else
		{
			m_Agents[idx]->SetRenderBehavior(false);
			m_Agents[0]->SetRenderBehavior(true);

		}


		// Show neighborhood circle + change color to green if in it
		if (m_NeighborhoodDebug)
		{
			DEBUGRENDERER2D->DrawCircle(m_Agents[0]->GetPosition(), m_NeighborhoodRadius, { 1.0f, 1.0f, 1.0f, 1.0f }, 0.0f);


			// Calculate distance between agent[0] and 1 out of the flock
			float distance{ (m_Agents[0]->GetPosition() - m_Agents[idx]->GetPosition()).Magnitude() };

			// check for self assignment
			if (m_Agents[idx] != m_Agents[0])
			{
				// If  0 < distance < m_NeighborhoodRadius
				if ((distance > 0) && (distance < m_NeighborhoodRadius))
				{
					m_Agents[idx]->SetBodyColor({ 0.0f, 1.0f, 0.0f, 1.0f });
				}
				else
				{
					m_Agents[idx]->SetBodyColor({ 1.0f, 1.0f, 0.0f, 1.0f });
				}
			}
		}
		else
		{
			m_Agents[idx]->SetBodyColor({ 1.0f, 1.0f, 0.0f, 1.0f });
		}
	}
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
	ImGui::Checkbox("Spatial Partitioning", &m_SpatialPartitioning);


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
