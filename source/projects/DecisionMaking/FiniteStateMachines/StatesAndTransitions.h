/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// StatesAndTransitions.h: Implementation of the state/transition classes
/*=============================================================================*/
#ifndef ELITE_APPLICATION_FSM_STATES_TRANSITIONS
#define ELITE_APPLICATION_FSM_STATES_TRANSITIONS

#include "projects/Shared/Agario/AgarioAgent.h"
#include "projects/Shared/Agario/AgarioFood.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "framework/EliteAI/EliteData/EBlackboard.h"

//------------
//---STATES---
//------------
namespace FSMState
{
	// First State
	class WanderState : public Elite::FSMState
	{
	public:
		WanderState() : FSMState() {};
		virtual void OnEnter(Elite::Blackboard* pBlackBoard) override;


	};

	// Second State
	class SeekFoodState : public Elite::FSMState
	{
	public:
		SeekFoodState() : FSMState() {};
		virtual void OnEnter(Elite::Blackboard* pBlackBoard) override;

	};

	// Third State
	class EvadeState : public Elite::FSMState
	{
	public:
		EvadeState() : FSMState() {};
		virtual void OnEnter(Elite::Blackboard* pBlackBoard) override;

	};
}
//-----------------
//---TRANSITIONS---
//-----------------
namespace FSMConditions
{
	// First Condition
	class FoodNearByCondition : public Elite::FSMCondition
	{
	public:

		FoodNearByCondition() : FSMCondition() {};

		// Inherited via FSMCondition
		virtual bool Evaluate(Elite::Blackboard* pBlackboard) const override;

	};

	class NoFoodNearByCondition : public Elite::FSMCondition
	{
	public:

		NoFoodNearByCondition() : FSMCondition() {};

		// Inherited via FSMCondition
		virtual bool Evaluate(Elite::Blackboard* pBlackboard) const override;

	};


	// Second Condition
	class EvadeAgentCondition : public Elite::FSMCondition
	{
	public:

		EvadeAgentCondition() : FSMCondition() {};

		// Inherited via FSMCondition
		virtual bool Evaluate(Elite::Blackboard* pBlackboard) const override;

	};

	class NoBiggerAgentNearByCondition : public Elite::FSMCondition
	{
	public:

		NoBiggerAgentNearByCondition() : FSMCondition() {};

		// Inherited via FSMCondition
		virtual bool Evaluate(Elite::Blackboard* pBlackboard) const override;

	};

	// Third condition
}


#endif