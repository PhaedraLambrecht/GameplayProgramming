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

}
//-----------------
//---TRANSITIONS---
//-----------------
namespace FSMConditions
{
	// First Condition
	class FoodNearBYCondition : public Elite::FSMCondition
	{
	public:

		FoodNearBYCondition() : FSMCondition() {};

		// Inherited via FSMCondition
		virtual bool Evaluate(Elite::Blackboard* pBlackboard) const override;

	};
	class NoFoodNearBYCondition : public Elite::FSMCondition
	{
	public:

		NoFoodNearBYCondition() : FSMCondition() {};

		// Inherited via FSMCondition
		virtual bool Evaluate(Elite::Blackboard* pBlackboard) const override;

	};

}


#endif