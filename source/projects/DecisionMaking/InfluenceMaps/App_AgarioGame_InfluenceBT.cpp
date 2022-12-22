#include "stdafx.h"
#include "App_AgarioGame_InfluenceBT.h"
#include "BehaviorsInfluence.h"

#include "projects/Shared/Agario/AgarioFood.h"
#include "projects/Shared/Agario/AgarioAgent.h"
#include "projects/Shared/Agario/AgarioContactListener.h"
#include "projects/Shared/NavigationColliderElement.h"


using namespace Elite;
App_AgarioGame_InfluenceBT::App_AgarioGame_InfluenceBT()
{
	// Initialize the Influence grid/ graph
	m_pInfluenceGrid = new InfluenceMap<InfluenceGrid>(true);
	m_pInfluenceGrid->InitializeGrid(15, 15, 10, false, true);
	m_pInfluenceGrid->InitializeBuffer();

	m_GraphRenderer.SetNumberPrintPrecision(0);
}

App_AgarioGame_InfluenceBT::~App_AgarioGame_InfluenceBT()
{

	for (auto& f : m_pFoodVec)
	{
		SAFE_DELETE(f);
	}
	m_pFoodVec.clear();

	for (auto& a : m_pAgentVec)
	{
		SAFE_DELETE(a);
	}
	m_pAgentVec.clear();

	SAFE_DELETE(m_pContactListener);
	SAFE_DELETE(m_pSmartAgent);

	for (auto pNC : m_vNavigationColliders)
		SAFE_DELETE(pNC);
	m_vNavigationColliders.clear();

	SAFE_DELETE(m_pInfluenceGrid);
}

void App_AgarioGame_InfluenceBT::Start()
{
	//Set Camera
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(m_TrimWorldSize * .5f, m_TrimWorldSize * .5f));
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(m_TrimWorldSize * .55f);


	//Create Boundaries
	const float blockSize{ 2.0f };
	const float hBlockSize{ blockSize / 2.0f };
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(-hBlockSize, m_TrimWorldSize * .5f), blockSize, m_TrimWorldSize + blockSize * 2.0f));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(m_TrimWorldSize + hBlockSize, m_TrimWorldSize * .5f), blockSize, m_TrimWorldSize + blockSize * 2.0f));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(m_TrimWorldSize * .5f, m_TrimWorldSize + hBlockSize), m_TrimWorldSize, blockSize));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(m_TrimWorldSize * .5f, -hBlockSize), m_TrimWorldSize, blockSize));


	//Creating the world contact listener that informs us of collisions
	m_pContactListener = new AgarioContactListener();


	//Create food items
	m_pFoodVec.reserve(m_AmountOfFood);
	for (int i = 0; i < m_AmountOfFood; i++)
	{
		Elite::Vector2 randomPos = randomVector2(0.f, m_TrimWorldSize);
		m_pFoodVec.push_back(new AgarioFood(randomPos));
	}


	//Create agents
	m_pAgentVec.reserve(m_AmountOfAgents);
	for (int i = 0; i < m_AmountOfAgents; i++)
	{
		Vector2 randomPos = randomVector2(0.f, m_TrimWorldSize);
		AgarioAgent* newAgent = new AgarioAgent(randomPos);

		//1. Create Blackboard
		Blackboard* pBlackBoard = CreateBlackboard(newAgent);

		//2. Create BehaviorTree
		BehaviorTree* pBehaviorTree = new BehaviorTree(pBlackBoard,
			new BehaviorSelector(
				{
				// Try to seek Food
				new BehaviorSequence(
					{
						new BehaviorConditional(BHTI_Conditions::IsFoodNearby),
						new BehaviorAction(BHTI_Actions::ChangeToSeek)
					}),
				// Fall back to wander
				new BehaviorAction(BHTI_Actions::ChangeToWander)
				}
		));

		//3. Set the BehaviorTree active on the agent 
		newAgent->SetDecisionMaking(pBehaviorTree);



		m_pAgentVec.push_back(newAgent);
	}



	//-------------------
	//Create The Smart Agent
	//-------------------
	Elite::Vector2 randomPos = randomVector2(0.f, m_TrimWorldSize);
	Color customColor = Color{ randomFloat(), randomFloat(), randomFloat() };
	m_pSmartAgent = new AgarioAgent(randomPos, customColor);

	//Create and add the necessary blackboard data
	//1. Create Blackboard
	Blackboard* pBlackBoard = CreateBlackboard(m_pSmartAgent);

	//2. Create BehaviorTree (make more conditions/actions and create a more advanced tree than the simple agents

	BehaviorTree* pBehaviorTree = new BehaviorTree(pBlackBoard,
		new BehaviorSelector(
			{
			new BehaviorSequence(
			{
				new BehaviorConditional(BHTI_Conditions::IsBiggerAgentNearby),
				new BehaviorAction(BHTI_Actions::ChangeToEvade)
			}),

			// Try to evade bigger agents
			new BehaviorSequence(
			{
				new BehaviorConditional(BHTI_Conditions::IsSmallerAgentNearby),
				new BehaviorAction(BHTI_Actions::ChangeToPursuit)
			}),

			// Try to seek Food
			new BehaviorSequence(
				{
					new BehaviorConditional(BHTI_Conditions::IsFoodNearby),
					new BehaviorAction(BHTI_Actions::ChangeToSeek)
				}),
			// Fall back to wander
			new BehaviorAction(BHTI_Actions::ChangeToWander)
			}
	));

	//3. Set the BehaviorTree active on the agent 
	m_pSmartAgent->SetDecisionMaking(pBehaviorTree);
	m_pSmartAgent->SetRenderBehavior(true);
}

void App_AgarioGame_InfluenceBT::Update(float deltaTime)
{
	UpdateImGui();

	AddFoodInfluance();
	AddSmallAgentInfluance();
	AddBiggAgentInfluance();


		//Check if agent is still alive
	if (m_pSmartAgent->CanBeDestroyed())
	{
		m_GameOver = true;
		return;
	}

	//Update the custom agent
	m_pSmartAgent->Update(deltaTime);

	//Update the other agents and food
	UpdateAgarioEntities(m_pFoodVec, deltaTime);
	UpdateAgarioEntities(m_pAgentVec, deltaTime);


	//Check if we need to spawn new food
	m_TimeSinceLastFoodSpawn += deltaTime;
	if (m_TimeSinceLastFoodSpawn > m_FoodSpawnDelay)
	{
		m_TimeSinceLastFoodSpawn = 0.f;
		m_pFoodVec.push_back(new AgarioFood(randomVector2(0.f, m_TrimWorldSize)));
	}


	// Update the grid
	m_GridEditor.UpdateGraph(m_pInfluenceGrid);
	m_pInfluenceGrid->PropagateInfluence(deltaTime);
}

void App_AgarioGame_InfluenceBT::Render(float deltaTime) const
{
	RenderWorldBounds(m_TrimWorldSize);

	for (AgarioFood* f : m_pFoodVec)
	{
		f->Render(deltaTime);
	}

	for (AgarioAgent* a : m_pAgentVec)
	{
		a->Render(deltaTime);
	}

	m_pSmartAgent->Render(deltaTime);


	m_pInfluenceGrid->SetNodeColorsBasedOnInfluence();
	m_GraphRenderer.RenderGraph(m_pInfluenceGrid, true, false, false, true);
	//m_GraphRenderer.RenderGraph(m_pInfluenceGrid, true, true);


}

Blackboard* App_AgarioGame_InfluenceBT::CreateBlackboard(AgarioAgent* a)
{
	Elite::Blackboard* pBlackboard = new Elite::Blackboard();
	pBlackboard->AddData("Agent", a);
	pBlackboard->AddData("AgentsVec", &m_pAgentVec);
	pBlackboard->AddData("FoodVec", &m_pFoodVec);
	pBlackboard->AddData("WorldSize", m_TrimWorldSize);
	pBlackboard->AddData("Target", Elite::Vector2{});
	pBlackboard->AddData("AgentFleeTarget", static_cast<AgarioAgent*>(nullptr)); // Needs the cast for the type
	pBlackboard->AddData("Time", 0.0f);
	pBlackboard->AddData("InfluenceGrid", m_pInfluenceGrid);

	return pBlackboard;
}

void App_AgarioGame_InfluenceBT::UpdateImGui()
{
	//------- UI --------
#ifdef PLATFORM_WINDOWS
#pragma region UI
	{
		//Setup
		int menuWidth = 150;
		int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
		int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
		bool windowActive = true;
		ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
		ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 90));
		ImGui::Begin("Agario", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::PushAllowKeyboardFocus(false);
		ImGui::SetWindowFocus();
		ImGui::PushItemWidth(70);
		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
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
		ImGui::Spacing();

		ImGui::Text("Agent Info");
		ImGui::Text("Radius: %.1f", m_pSmartAgent->GetRadius());
		ImGui::Text("Survive Time: %.1f", TIMER->GetTotal());


		auto momentum = m_pInfluenceGrid->GetMomentum();
		auto decay = m_pInfluenceGrid->GetDecay();
		auto propagationInterval = m_pInfluenceGrid->GetPropagationInterval();

		ImGui::SliderFloat("Momentum", &momentum, 0.0f, 1.f, "%.2");
		ImGui::SliderFloat("Decay", &decay, 0.f, 1.f, "%.2");
		ImGui::SliderFloat("Propagation Interval", &propagationInterval, 0.f, 2.f, "%.2");
		ImGui::Spacing();

		//Set data
		m_pInfluenceGrid->SetMomentum(momentum);
		m_pInfluenceGrid->SetDecay(decay);
		m_pInfluenceGrid->SetPropagationInterval(propagationInterval);



		//End
		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
	if (m_GameOver)
	{
		//Setup
		int menuWidth = 300;
		int menuHeight = 100;
		int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
		int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
		bool windowActive = true;
		ImGui::SetNextWindowPos(ImVec2(width / 2.0f - menuWidth, height / 2.0f - menuHeight));
		ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)menuHeight));
		ImGui::Begin("Game Over", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ImGui::Text("Final Agent Info");
		ImGui::Text("Radius: %.1f", m_pSmartAgent->GetRadius());
		ImGui::Text("Survive Time: %.1f", TIMER->GetTotal());
		ImGui::End();
	}
#pragma endregion
#endif

}



void App_AgarioGame_InfluenceBT::AddFoodInfluance()
{
	for (const auto& pTreat : m_pFoodVec)
	{

			m_pInfluenceGrid->SetInfluenceAtPosition
			(
				pTreat->GetPosition(),
				50.0f
			);
	
	}
}

void App_AgarioGame_InfluenceBT::AddSmallAgentInfluance()
{
	const float minPursuitRadius{ 2.0f };  // min size diff before persuing
	for (const auto& pTreat : m_pAgentVec)
	{
		float sizeDif( m_pSmartAgent->GetRadius() - pTreat->GetRadius() );

		if (sizeDif >= minPursuitRadius)
		{
			m_pInfluenceGrid->SetInfluenceAtPosition
			(
				pTreat->GetPosition(),
				50.0f
			);
		}
	}
}

void App_AgarioGame_InfluenceBT::AddBiggAgentInfluance()
{
	const float minEvadeRadius{ -2.0f };
	for (const auto& enemy : m_pAgentVec)
	{
		float sizeDif(m_pSmartAgent->GetRadius() - enemy->GetRadius());

		if (sizeDif < minEvadeRadius)
		{
			m_pInfluenceGrid->SetInfluenceAtPosition
			(
				enemy->GetPosition(),
				-60.0f
			);
		}
	}
}

