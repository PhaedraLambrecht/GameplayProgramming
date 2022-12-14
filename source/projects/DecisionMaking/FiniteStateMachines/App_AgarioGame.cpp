#include "stdafx.h"
#include "App_AgarioGame.h"
#include "StatesAndTransitions.h"


//AgarioIncludes
#include "projects/Shared/Agario/AgarioFood.h"
#include "projects/Shared/Agario/AgarioAgent.h"
#include "projects/Shared/Agario/AgarioContactListener.h"

using namespace Elite;
using namespace FSMState;
using namespace FSMConditions;


App_AgarioGame::App_AgarioGame()
{
}

App_AgarioGame::~App_AgarioGame()
{
	for (auto& f : m_pFoodVec)
	{
		SAFE_DELETE(f);
	}
	m_pFoodVec.clear();

	for (auto& a : m_pDumbAgentVec)
	{
		SAFE_DELETE(a);
	}
	m_pDumbAgentVec.clear();

	SAFE_DELETE(m_pContactListener);
	SAFE_DELETE(m_pSmartAgent);
	for (auto& s : m_pStates)
	{
		SAFE_DELETE(s);
	}

	for (auto& t : m_pConditions)
	{
		SAFE_DELETE(t);
	}

}

void App_AgarioGame::Start()
{
	//Creating the world contact listener that informs us of collisions
	m_pContactListener = new AgarioContactListener();

	//Create food items
	m_pFoodVec.reserve(m_AmountOfFood);
	for (int i = 0; i < m_AmountOfFood; i++)
	{
		Elite::Vector2 randomPos = randomVector2(0, m_TrimWorldSize);
		m_pFoodVec.push_back(new AgarioFood(randomPos));
	}


	WanderState* pWanderState = new WanderState();
	m_pStates.push_back(pWanderState);

	//Create default agents
	m_pDumbAgentVec.reserve(m_AmountOfAgents);
	for (int i = 0; i < m_AmountOfAgents; i++)
	{
		Elite::Vector2 randomPos = randomVector2(0, m_TrimWorldSize * (2.0f / 3));
		AgarioAgent* newAgent = new AgarioAgent(randomPos);



		// HandsOn - dumb agent
		Blackboard* pBlackBoard = CreateBlackboard(newAgent);

		FiniteStateMachine* pStateMachine = new FiniteStateMachine(pWanderState, pBlackBoard);
		newAgent->SetDecisionMaking(pStateMachine);



		m_pDumbAgentVec.push_back(newAgent);
	}



	//-------------------
	//Create Custom Agent
	//-------------------
	Elite::Vector2 randomPos = randomVector2(0, m_TrimWorldSize * (2.0f / 3));
	Color customColor = Color{ 0.0f, 1.0f, 0.0f };
	m_pSmartAgent = new AgarioAgent(randomPos, customColor);
	m_pSmartAgent->SetRenderBehavior(true);

	//1. Create and add the necessary blackboard data
	Blackboard* pBlackBoard = CreateBlackboard(m_pSmartAgent);


	//2. Create the different agent states
	// Food State
	SeekFoodState* pSeekFoodState = new SeekFoodState();
	m_pStates.push_back(pSeekFoodState);

	// Evdade State
	EvadeState* pEvadeState = new EvadeState();
	m_pStates.push_back(pEvadeState);

	// Pursue State
	PursueState* pPursueState = new PursueState();
	m_pStates.push_back(pPursueState);


	//3. Create the conditions beetween those states
	// Food Conditions
	FoodNearByCondition* pFoodNearByCondition = new FoodNearByCondition(); // If there is food
	m_pConditions.push_back(pFoodNearByCondition);

	NoFoodNearByCondition* pNoFoodNearByCondition = new NoFoodNearByCondition(); // If there is no food
	m_pConditions.push_back(pNoFoodNearByCondition);


	// EvadeCondition
	EvadeAgentCondition* pEvadeCondition = new EvadeAgentCondition(); // If there is a bigger agent
	m_pConditions.push_back(pEvadeCondition);

	NoBiggerAgentNearByCondition* pNoEvadeCondition = new NoBiggerAgentNearByCondition(); // If there is no bigger agent nearby
	m_pConditions.push_back(pNoEvadeCondition);

	// PursueCondition
	SmallerAgentNearByCondition* pPursueCondition = new SmallerAgentNearByCondition();
	m_pConditions.push_back(pPursueCondition);

	NoSmallerAgentNearByCondition* pNoPursueCondition = new NoSmallerAgentNearByCondition();
	m_pConditions.push_back(pNoPursueCondition);



	//4. Create the finite state machine with a starting state and the blackboard
	FiniteStateMachine* pStateMachine = new FiniteStateMachine(pWanderState, pBlackBoard);



	//5. Add the transitions for the states to the state machine
	// stateMachine->AddTransition(startState, toState, condition)
	// startState: active state for which the transition will be checked
	// condition: if the Evaluate function returns true => transition will fire and move to the toState
	// toState: end state where the agent will move to if the transition fires

	// Swapping back to wander
	pStateMachine->AddTransition(pSeekFoodState, pWanderState, pNoFoodNearByCondition); // seek food --> wander
	pStateMachine->AddTransition(pEvadeState, pWanderState, pNoEvadeCondition); // evade --> wander
	pStateMachine->AddTransition(pPursueState, pWanderState, pNoPursueCondition); // pursue --> wander


	// Seeking food
	pStateMachine->AddTransition(pWanderState, pSeekFoodState, pFoodNearByCondition); // wander --> seek food

	// Evading agents
	pStateMachine->AddTransition(pWanderState, pEvadeState, pEvadeCondition); // wander --> evade
	pStateMachine->AddTransition(pSeekFoodState, pEvadeState, pEvadeCondition); // seek food --> evade
	pStateMachine->AddTransition(pPursueState, pEvadeState, pEvadeCondition); // pursue --> evade

	//// Pursue agents
	pStateMachine->AddTransition(pWanderState, pPursueState, pPursueCondition); // wander --> pursue
	pStateMachine->AddTransition(pSeekFoodState, pPursueState, pPursueCondition); // wander --> pursue



	//6. Activate the decision making stucture on the custom agent by calling the SetDecisionMaking function
	m_pSmartAgent->SetDecisionMaking(pStateMachine);
}

void App_AgarioGame::Update(float deltaTime)
{
	UpdateImGui();
	//Check if agent is still alive
	if (m_pSmartAgent->CanBeDestroyed())
	{
		m_GameOver = true;

		//Update the other agents and food
		UpdateAgarioEntities(m_pFoodVec, deltaTime);
		UpdateAgarioEntities(m_pDumbAgentVec, deltaTime);
		return;
	}
	//Update the custom agent
	m_pSmartAgent->Update(deltaTime);
	m_pSmartAgent->TrimToWorld(m_TrimWorldSize, false);

	//Update the other agents and food
	UpdateAgarioEntities(m_pFoodVec, deltaTime);
	UpdateAgarioEntities(m_pDumbAgentVec, deltaTime);


	//Check if we need to spawn new food
	m_TimeSinceLastFoodSpawn += deltaTime;
	if (m_TimeSinceLastFoodSpawn > m_FoodSpawnDelay)
	{
		m_TimeSinceLastFoodSpawn = 0.f;
		m_pFoodVec.push_back(new AgarioFood(randomVector2(0, m_TrimWorldSize)));
	}
}

void App_AgarioGame::Render(float deltaTime) const
{
	RenderWorldBounds(m_TrimWorldSize);

	for (AgarioFood* f : m_pFoodVec)
	{
		f->Render(deltaTime);
	}

	for (AgarioAgent* a : m_pDumbAgentVec)
	{
		a->Render(deltaTime);
	}

	m_pSmartAgent->Render(deltaTime);
}

Blackboard* App_AgarioGame::CreateBlackboard(AgarioAgent* a)
{
	Blackboard* pBlackboard = new Blackboard();
	pBlackboard->AddData("Agent", a);

	// Food
	pBlackboard->AddData("FoodVecPtr", &m_pFoodVec);
	pBlackboard->AddData("FoodNearByPtr", static_cast<AgarioFood*>(nullptr));

	// Evade
	pBlackboard->AddData("AgentVecPtr", &m_pDumbAgentVec);
	pBlackboard->AddData("BigAgent", static_cast<AgarioAgent*>(nullptr));

	// Pursue 
	pBlackboard->AddData("SmallAgent", static_cast<AgarioAgent*>(nullptr));
	//....



	return pBlackboard;
}

void App_AgarioGame::UpdateImGui()
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

		
		//ImGui::Checkbox("render behaviour", m_pSmartAgent->CanRenderBehavior())


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
