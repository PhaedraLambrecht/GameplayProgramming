//=== General Includes ===
#include "stdafx.h"
#include "EBehaviorTree.h"
using namespace Elite;

//-----------------------------------------------------------------
// BEHAVIOR TREE COMPOSITES (IBehavior)
//-----------------------------------------------------------------
#pragma region COMPOSITES
//SELECTOR
BehaviorState BehaviorSelector::Execute(Blackboard* pBlackBoard)
{	
	// Loop over all children in m_ChildBehaviors
	for (auto& child : m_ChildBehaviors)
	{
		//Every Child: Execute and store the result in m_CurrentState
		m_CurrentState = child->Execute(pBlackBoard);


		//// With if's
		////Check the currentstate and apply the selector Logic:
		////if a child returns Success:
		//if (m_CurrentState == BehaviorState::Success)
		//{
		//	//stop looping over all children and return Success
		//	return m_CurrentState;
		//}
		//
		////if a child returns Running:
		//if (m_CurrentState == BehaviorState::Running)
		//{
		//	//Running: stop looping and return Running
		//	return m_CurrentState;
		//}
		//
		////The selector fails if all children failed.
		//if (m_CurrentState == BehaviorState::Failure)
		//{
		//	continue;
		//}


		switch (m_CurrentState)
		{
		default:
		case Elite::BehaviorState::Failure:
			
			continue;
	
		case Elite::BehaviorState::Success:

			return m_CurrentState;

		case Elite::BehaviorState::Running:

			return m_CurrentState;
		}

	}

	//All children failed
	m_CurrentState = BehaviorState::Failure;
	return m_CurrentState;
}
//SEQUENCE
BehaviorState BehaviorSequence::Execute(Blackboard* pBlackBoard)
{
	//Loop over all children in m_ChildBehaviors
	for (auto& child : m_ChildBehaviors)
	{
		//Every Child: Execute and store the result in m_CurrentState
		m_CurrentState = child->Execute(pBlackBoard);


		//Check the currentstate and apply the sequence Logic:
		switch (m_CurrentState)
		{
		default:
		//The selector succeeds if all children succeeded.
		case Elite::BehaviorState::Success:
		
			continue;
			
		//if a child returns Failed:
		case Elite::BehaviorState::Failure:

			//stop looping over all children and return Failed
			return Elite::BehaviorState::Failure;

		//if a child returns Running:
		case Elite::BehaviorState::Running:

			//Running: stop looping and return Running
			return Elite::BehaviorState::Running;

		}
	}


	//All children succeeded 
	m_CurrentState = BehaviorState::Success;
	return m_CurrentState;
}
//PARTIAL SEQUENCE
BehaviorState BehaviorPartialSequence::Execute(Blackboard* pBlackBoard)
{
	while (m_CurrentBehaviorIndex < m_ChildBehaviors.size())
	{
		m_CurrentState = m_ChildBehaviors[m_CurrentBehaviorIndex]->Execute(pBlackBoard);
		switch (m_CurrentState)
		{
		case BehaviorState::Failure:

			m_CurrentBehaviorIndex = 0;
			return m_CurrentState;

		case BehaviorState::Success:

			++m_CurrentBehaviorIndex;

			m_CurrentState = BehaviorState::Running;

			return m_CurrentState;

		case BehaviorState::Running:

			return m_CurrentState;
		}
	}

	m_CurrentBehaviorIndex = 0;
	m_CurrentState = BehaviorState::Success;
	return m_CurrentState;
}
#pragma endregion
//-----------------------------------------------------------------
// BEHAVIOR TREE CONDITIONAL (IBehavior)
//-----------------------------------------------------------------
BehaviorState BehaviorConditional::Execute(Blackboard* pBlackBoard)
{
	if (m_fpConditional == nullptr)
		return BehaviorState::Failure;

	switch (m_fpConditional(pBlackBoard))
	{
	case true:
		m_CurrentState = BehaviorState::Success;
		return m_CurrentState;
	case false:
		m_CurrentState = m_CurrentState = BehaviorState::Failure;
		return m_CurrentState;
	}

	m_CurrentState = BehaviorState::Running;
	return m_CurrentState;
}
//-----------------------------------------------------------------
// BEHAVIOR TREE ACTION (IBehavior)
//-----------------------------------------------------------------
BehaviorState BehaviorAction::Execute(Blackboard* pBlackBoard)
{
	if (m_fpAction == nullptr)
		return BehaviorState::Failure;

	m_CurrentState = m_fpAction(pBlackBoard);
	return m_CurrentState;
}