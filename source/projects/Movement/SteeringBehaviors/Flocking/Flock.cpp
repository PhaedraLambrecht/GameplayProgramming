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
	, m_DefaultSeekWeight{ 0.01f }
	, m_DefaultSeparationWeight{ 0.45f }
	, m_DefaultCohesionWeight{ 0.42f }
	, m_DefaultAlignmentWeight{ 0.2f }
	, m_DefaultWanderWeight{ 0.6f }
{
	
	// initialize the partitioning
	m_pCellSpace = new CellSpace(m_WorldSize, m_WorldSize, 25, 25, m_FlockSize);


	InitializeBehaviours();

	InitializeAgents();


	// initialize the bools
	m_CanDebug = false;
	m_CanNeighborhoodDebug = false;
	m_CanSpatialPartitioning = true;

};

void Flock::InitializeAgents()
{
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
		m_pCellSpace->AddAgent(m_Agents[idx]);


		m_AgentOldPos[idx] = m_Agents[idx]->GetPosition();
	}

}

void Flock::InitializeBehaviours()
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
	weightedSteeringBehaviors.push_back({ m_pSeekBehavior, m_DefaultSeekWeight });
	weightedSteeringBehaviors.push_back({ m_pSeparationBehavior, m_DefaultSeparationWeight });
	weightedSteeringBehaviors.push_back({ m_pCohesionBehavior, m_DefaultCohesionWeight });
	weightedSteeringBehaviors.push_back({ m_pAlignmentBehavior, m_DefaultAlignmentWeight });
	weightedSteeringBehaviors.push_back({ m_pWanderBehavior, m_DefaultWanderWeight });
	m_pBlendedSteering = new BlendedSteering(weightedSteeringBehaviors);


	// Priority steering
	m_pPrioritySteering = new PrioritySteering({ m_pEvadeBehavior, m_pBlendedSteering });
}

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


	SAFE_DELETE(m_pCellSpace);
}


void Flock::Update(float deltaT)
{
	UpdateAgentToEvade();


	// Update the flock and AgentToEvade
	for (int idx{}; idx < m_Agents.size(); ++idx)
	{
		
		if(m_CanSpatialPartitioning) // If Spatial partitioning is used
		{
			UpdateCellSpace(idx, deltaT);
		}
		else // If Spatial Partitioning is not used
		{
			RegisterNeighbors(m_Agents[idx]);
			m_Agents[idx]->Update(deltaT);
		}


		UpdateDebugRendering(idx);

		// Updating the World Boundry
		if (m_TrimWorld)
		{
			m_Agents[idx]->TrimToWorld(m_WorldSize);
		}
	}
}

void Flock::UpdateAgentToEvade()
{
	if (m_pAgentToEvade != nullptr)
	{
		m_pEvadeBehavior->SetTarget(m_pAgentToEvade->GetPosition());
	}
}

void Flock::UpdateCellSpace(int index, float deltaT)
{
	m_pCellSpace->RegisterNeighbors(m_Agents[index], m_NeighborhoodRadius);
	m_pCellSpace->UpdateAgentCell(m_Agents[index], m_AgentOldPos[index]);
	RegisterNeighbors(m_Agents[index]);
	m_Agents[index]->Update(deltaT);


	m_AgentOldPos[index] = m_Agents[index]->GetPosition();
}

void Flock::UpdateDebugRendering(int index)
{
	// Shows render behavior
	if (m_CanDebug)
	{
		m_Agents[index]->SetRenderBehavior(true);
	}
	else
	{
		m_Agents[index]->SetRenderBehavior(false);
		m_Agents[m_Agents.size() - 1]->SetRenderBehavior(true);
	}
}


void Flock::UpdateAndRenderUI()
{
	SetupUI();
	DisplayUI();
	DisplayCheckBoxes();
	DisplaySliders();


	//End
	ImGui::PopAllowKeyboardFocus();
	ImGui::End();
	
}

void Flock::SetupUI()
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
}

void Flock::DisplayUI()
{
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
}

void Flock::DisplayCheckBoxes()
{
	//checkbox
	ImGui::Checkbox("Trim World", &m_TrimWorld);
	ImGui::Checkbox("Debug render Steering", &m_CanDebug);
	ImGui::Checkbox("Debug render Neighborhood", &m_CanNeighborhoodDebug);
	ImGui::Checkbox("Spatial Partitioning", &m_CanSpatialPartitioning);


	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
}

void Flock::DisplaySliders()
{
	ImGui::Text("Behavior Weights");
	ImGui::Spacing();


	//sliders to change the weight of the blended behavior
	ImGui::SliderFloat("Seek", &m_pBlendedSteering->GetWeightedBehaviorsRef()[0].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Separation", &m_pBlendedSteering->GetWeightedBehaviorsRef()[1].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Cohesion", &m_pBlendedSteering->GetWeightedBehaviorsRef()[2].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Alignment", &m_pBlendedSteering->GetWeightedBehaviorsRef()[3].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Wander", &m_pBlendedSteering->GetWeightedBehaviorsRef()[4].weight, 0.f, 1.f, "%.2");
}


void Flock::Render(float deltaT)
{
	//// Render flock and AgentToEvade
	//for(SteeringAgent* pAgent: m_Agents)
	//{
	//	pAgent->Render(deltaT);
	//}

	m_pAgentToEvade->Render(deltaT);


	for (int idx{}; idx < m_Agents.size(); ++idx)
	{
	
		if (m_CanNeighborhoodDebug && m_Agents[idx] == m_Agents[m_Agents.size() -1])
		{
			RenderNeighbourhood(deltaT);
		}

	}


	if (m_CanSpatialPartitioning)
	{
		m_pCellSpace->RenderCells();
	}

}

void Flock::RenderNeighbourhood(float deltaT)
{
	SteeringAgent*& agent{ m_Agents[m_Agents.size() -1] };


	DEBUGRENDERER2D->DrawCircle(agent->GetPosition(), m_NeighborhoodRadius, Color{ 0, 0, 1.0f }, 0);
	DEBUGRENDERER2D->DrawSolidCircle(agent->GetPosition(), 1.5f, Vector2{}, Color{ 0, 0, 1.0f }, -1);
	for (int neighbourIndex{ 0 }; neighbourIndex < GetNrOfNeighbors(); ++neighbourIndex)
	{
		const SteeringAgent* neighbour{ GetNeighbors()[neighbourIndex] };
		DEBUGRENDERER2D->DrawSolidCircle(neighbour->GetPosition(), 1.5f, Vector2{}, Color{ 0, 1.0f, 0 }, -1);
	}
}


void Flock::RegisterNeighbors(SteeringAgent* pAgent) // Neither work with the cellSpace
{
	m_NrOfNeighbors = 0;

	//// without the spatial partitioning (works fine)
	//for (const auto& agent : m_Agents)
	//{
	//	// Calculate distance between agent and 1 out of the flock
	//	float distance{ Distance(pAgent->GetPosition(), agent->GetPosition()) };
	//
	//	// check for self assignment
	//	if (agent != pAgent)
	//	{
	//		// If  0 < distance < m_NeighborhoodRadius
	//		if ((distance > 0) && (distance < m_NeighborhoodRadius))
	//		{
	//			m_Neighbors[m_NrOfNeighbors] = agent;
	//			++m_NrOfNeighbors;
	//		}
	//	}
	//}


	// Very weird behaviour with spatial partitioning, works without spatial partitioning
	if(m_CanSpatialPartitioning)
	{
		for (int idx{}; idx < m_pCellSpace->GetNrOfNeighbors(); ++idx)
		{
			// Calculate distance between agent and 1 out of the flock
			Elite::Vector2 distance{ m_pCellSpace->GetNeighbors()[idx]->GetPosition() - pAgent->GetPosition() };
			
			// check for self assignment
			if (m_pCellSpace->GetNeighbors()[idx] != pAgent)
			{
				// If  0 < distance < m_NeighborhoodRadius
				if (distance.Magnitude() <= m_NeighborhoodRadius)
				{
					m_Neighbors[m_NrOfNeighbors] = m_pCellSpace->GetNeighbors()[idx];
					++m_NrOfNeighbors;
				}
			}
		}
	}
	else
	{
		for (const auto& agent: m_Agents)
		{
			// Calculate distance between agent and 1 out of the flock
			float distance{ Distance(pAgent->GetPosition(), agent->GetPosition())};
	
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


	//// does not move for both (with the flock behaviours)
	//if (m_CanSpatialPartitioning)
	//{
	//	m_pCellSpace->RegisterNeighbors(pAgent, m_NeighborhoodRadius);
	//}
	//
	//const std::vector<SteeringAgent*>& agentsList{ (m_CanSpatialPartitioning ? m_pCellSpace->GetNeighbors() : m_Agents) };
	//const size_t agentListSize{ m_CanSpatialPartitioning ? m_pCellSpace->GetNrOfNeighbors() : m_Agents.size() };
	//m_NrOfNeighbors = 0;
	//for (size_t agentIndex{ 0 }; agentIndex < agentListSize; ++agentIndex)
	//{
	//	if (pAgent != agentsList[agentIndex])
	//	{
	//		float distanceSquared{ DistanceSquared(agentsList[agentIndex]->GetPosition(), pAgent->GetPosition()) };
	//		if (distanceSquared <= m_NeighborhoodRadius * m_NeighborhoodRadius)
	//		{
	//			m_Neighbors[m_NrOfNeighbors] = agentsList[agentIndex];
	//			++m_NrOfNeighbors;
	//		}
	//	}
	//}
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
	Vector2 avarage{};


	for (size_t idx{}; idx < m_NrOfNeighbors; ++idx)
	{
		avarage += m_Neighbors[idx]->GetLinearVelocity();
	}
	avarage /= float(m_NrOfNeighbors);


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