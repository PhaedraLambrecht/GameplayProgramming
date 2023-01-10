//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "FlowAgent.h"
#include "App_Flowfield.h"


using namespace Elite;

//Destructor
App_FlowField::~App_FlowField()
{
	for (auto& agent : *m_pAgents)
	{
		SAFE_DELETE(agent);
	}
	SAFE_DELETE(m_pAgents);


	SAFE_DELETE(m_pGrid);
}


//Functions
void App_FlowField::Start()
{
	m_pAgents = new std::vector< FlowAgent*>();


	m_TrimWorldSize = 50.f;
	float worldDimensions{ 100.0f };
	m_pGrid = new Grid(Elite::Vector2(worldDimensions, worldDimensions), Elite::Vector2(10.f, 10.f));
}

void App_FlowField::Update(float deltaTime)
{
	//INPUT
	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))// If left mousebutton is released
	{
		// Set the mouse data/pos
		auto const mouseData = INPUTMANAGER->GetMouseData(InputType::eMouseButton, InputMouseButton::eLeft);
		auto mousePos{ DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) }) };

		// If not true, add obstacle
		if (!m_AreObstaclesMade)
			m_pGrid->AddObstacle(mousePos);

	}
	else if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eMiddle))// If middle mousebutton is released
	{
		// Set the mouse data/pos
		auto const mouseData = INPUTMANAGER->GetMouseData(InputType::eMouseButton, InputMouseButton::eMiddle);
		auto mousePos{ DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) }) };

		// if not true, add goal
		if (!m_IsGoalMade)
			m_pGrid->AddGoal(mousePos);

	}

	//IMGUI
	UpdateImGui();

	m_pGrid->Update(deltaTime);










	bool hasReachedGoal{};
	Elite::Vector2 agentPos{};
	int numAgentsToBeRemoved{};

	// Iterate over all the agents
	for (auto& agent : *m_pAgents)
	{
		agentPos = agent->GetPos();


		// Check if the agent has reached the goal
		hasReachedGoal = m_pGrid->AgentReachedGoal(agentPos);
		agent->SetHasReachedGoal(hasReachedGoal);




		// Move to the next square if they haven't reached the goal
		if (agent->GetHasReachedGoal() == false)
		{
			m_pGrid->MoveToNextSquare(agentPos, agent->GetTargetPos(), agent->GetFirstMove());
		}


		agent->Update(deltaTime);


		// Check if agent needs to be removed
		if (agent->GetNeedsToGo())
		{
			++numAgentsToBeRemoved;
		}
	
		// Remove agents that need to go
		for (int idx{}; idx < numAgentsToBeRemoved; ++idx)
		{
			// then erase it
			m_pAgents->erase(
			// remove if the agent needs to be removed
			std::remove_if
			(
				m_pAgents->begin(), 
				m_pAgents->end(), 
				[](FlowAgent* agent)
			{
				if (agent->GetNeedsToGo())
				{
					delete agent;
					return true;
				}
				else
				{
					return false;
				}
				
				}
			), 
			m_pAgents->end());
		}



	}

}

void App_FlowField::Render(float deltaTime) const
{
	m_pGrid->Render(deltaTime);

	for (auto& agent : *m_pAgents)
	{
		agent->Render(deltaTime);
	}
	

	if (m_TrimWorld)
	{
		std::vector<Elite::Vector2> points =
		{
			{ -m_TrimWorldSize, m_TrimWorldSize },
			{ m_TrimWorldSize, m_TrimWorldSize },
			{ m_TrimWorldSize, -m_TrimWorldSize },
			{ -m_TrimWorldSize, -m_TrimWorldSize }
		};
		DEBUGRENDERER2D->DrawPolygon(&points[0], 4, { 1, 1, 1, 1 }, 0.4f);

		
	}


}


//------------------
// Helper functins
//------------------
void App_FlowField::UpdateImGui()
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		int menuWidth = 200;
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
		ImGui::Text("LMB: place Obstacle");
		ImGui::Text("MMB: place target");
		ImGui::Text("RMB: move cam");
		ImGui::Text("Scrollwheel: zoom cam.");
		ImGui::Unindent();


		/*Spacing*/
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();


		/*Spacing*/
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Flow fields");
		ImGui::Spacing();
		ImGui::Spacing();



		// This goddamn shit took me 30 min to figure out (on top of the 2 days earlier this semester)
		ImGui::Text("Getting ready");

		// obstacles
		ImGui::Checkbox("Obstacles ready", &m_AreObstaclesMade);

		//goals
		ImGui::Checkbox("Goals ready", &m_IsGoalMade);

		// Allowing agents to spawn when needed
		ImGui::Checkbox("Spawn agents", &m_AreAgentsSpawned);


		// Make flow fields
		if (m_IsGoalMade && m_AreObstaclesMade)
		{
			ImGui::Checkbox("Made flowfields", &m_IsFlowFieldMade);

			if (m_IsFlowFieldMade)
			{
				m_pGrid->CreateGoal();
				m_pGrid->ToggleDrawObstacles(m_IsObstacleDrawn);
			}


			// Max 50 agents
			if (m_AreAgentsSpawned && (m_AgentSpawns != m_MaxAgentSpawns))
			{
				SpawnAgents();
				++m_AgentSpawns;
				m_AreAgentsSpawned = false;
			}

		}



		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("Visualization");
	//	ImGui::Checkbox("Trim World", &m_TrimWorld);

		// Toggle the grid
		ImGui::Checkbox("Draw grid", &m_IsGridDrawn);
		m_pGrid->ToggleDrawGrid(m_IsGridDrawn);

		// Toggle the obstacles
		ImGui::Checkbox("Draw obstacles", &m_IsObstacleDrawn);
		m_pGrid->ToggleDrawObstacles(m_IsObstacleDrawn);

		// Toggle the goal(s)
		ImGui::Checkbox("Draw goals", &m_IsGoalDrawn);
		m_pGrid->ToggleDrawGoal(m_IsGoalDrawn);

		// Toggle the directions
		ImGui::Checkbox("Draw direction", &m_IsDirectionDrawn);
		m_pGrid->ToggleDrawDirections(m_IsDirectionDrawn);



		ImGui::Spacing();
		ImGui::Separator();


		//End
		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
#pragma endregion
#endif
}

void App_FlowField::SpawnAgents()
{
	for (int agentIdx{}; agentIdx < m_MaxAgents; ++agentIdx)
	{
		m_pAgents->push_back(new FlowAgent(m_pGrid->GetRandomPos()));
	}
}
