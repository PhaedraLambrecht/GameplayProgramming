//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_Flowfield.h"


using namespace Elite;

//Destructor
App_FlowField::~App_FlowField()
{
	SAFE_DELETE(m_pGrid);
}


//Functions
void App_FlowField::Start()
{
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
		if (!m_MadeObstacles)
			m_pGrid->AddObstacle(mousePos);

	}
	else if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eMiddle))// If middle mousebutton is released
	{
		// Set the mouse data/pos
		auto const mouseData = INPUTMANAGER->GetMouseData(InputType::eMouseButton, InputMouseButton::eMiddle);
		auto mousePos{ DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) }) };
		
		// if not true, add goal
		if (!m_MadeGoals)
			m_pGrid->AddGoal(mousePos);

	}

	//IMGUI
	UpdateImGui();

	m_pGrid->Update(deltaTime);
}

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
		ImGui::Text("LMB: place target");
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
		ImGui::Checkbox("Obstacles ready", &m_MadeObstacles);

		//goals
		ImGui::Checkbox("Goals ready", &m_MadeGoals);


		// Make flow fields
		if ( m_MadeGoals && m_MadeObstacles )
		{
			ImGui::Checkbox("Made flowfields", &m_MadeFlowfield);

			if (m_MadeFlowfield)
			{
				m_pGrid->CreateGoal();
				m_pGrid->ToggleDrawObstacles(m_IsObstacleDrawn);
			}

		}


		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("Visualization");
		//ImGui::Checkbox("Trim World", &m_TrimWorld);

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


void App_FlowField::Render(float deltaTime) const
{
	m_pGrid->Render(deltaTime);

	if (m_TrimWorld)
	{
		std::vector<Elite::Vector2> points =
		{
			{ -m_TrimWorldSize, m_TrimWorldSize },
			{ m_TrimWorldSize, m_TrimWorldSize },
			{ m_TrimWorldSize, -m_TrimWorldSize },
			{ -m_TrimWorldSize, -m_TrimWorldSize }
		};
		DEBUGRENDERER2D->DrawPolygon(&points[0], 4, { 1, 0, 0, 1 }, 0.4f);
	}
}
