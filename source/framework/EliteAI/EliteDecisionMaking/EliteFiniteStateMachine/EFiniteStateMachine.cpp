//=== General Includes ===
#include "stdafx.h"
#include "EFiniteStateMachine.h"
using namespace Elite;

FiniteStateMachine::FiniteStateMachine(FSMState* startState, Blackboard* pBlackboard)
	: m_pCurrentState(nullptr),
	m_pBlackboard(pBlackboard)
{
	ChangeState(startState);
}

FiniteStateMachine::~FiniteStateMachine()
{
	SAFE_DELETE(m_pBlackboard);
}

void FiniteStateMachine::AddTransition(FSMState* startState, FSMState* toState, FSMCondition* condition)
{
	auto it = m_Transitions.find(startState);
	if (it == m_Transitions.end())
	{
		m_Transitions[startState] = Transitions();
	}

	m_Transitions[startState].push_back(std::make_pair(condition, toState));
}

void FiniteStateMachine::Update(float deltaTime)
{
	// Look if 1 or more transitions exists for the current state that we are in --> Check the transitions map for a TransitionState pair
	auto transitionIt{ m_Transitions.find(m_pCurrentState) };
	

	// If a TransitionState exists
	if (transitionIt != m_Transitions.end())// if not found, it gives the end
	{
		// Loop over all the TransitionState pairs 
		for (const auto& transition : transitionIt->second)
		{
			FSMCondition* condition{ transition.first };
			FSMState* state{ transition.second };

			
			// If the Evaluate function of the FSMCondition returns true => transition to the new corresponding state
			if (condition->Evaluate(m_pBlackboard))
			{
				ChangeState(state);

				break;
			}
		}
	}

	// Update the current state (if one exists)
	m_pCurrentState->Update(m_pBlackboard, deltaTime);
}

Blackboard* FiniteStateMachine::GetBlackboard() const
{
	return m_pBlackboard;
}

void FiniteStateMachine::ChangeState(FSMState* newState)
{
	// 1. If currently in a state => make sure the OnExit of that state gets called
	if (m_pCurrentState != nullptr)
	{
		m_pCurrentState->OnExit(m_pBlackboard);
	}


	// 2. Change the current state to the new state
	m_pCurrentState = newState;


	// 3. Call the OnEnter of the new state 
	if (m_pCurrentState == nullptr)// -- > checks for the user doing someting wrong
	{
		std::cout << "Changed state received a nullptr instead of a valid state" << '\n';
		return;
	}
	m_pCurrentState->OnEnter(m_pBlackboard);

}
