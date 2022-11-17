#include "stdafx.h"
#include "StatesAndTransitions.h"

using namespace Elite;
using namespace FSMState;


void WanderState::OnEnter(Blackboard* pBlackBoard)
{
	AgarioAgent* pAgent;


	// error checking
	bool isValid = pBlackBoard->GetData("Agent", pAgent);
	if (isValid == false || pAgent == nullptr)
	{
		return;
	}


	pAgent->SetToWander();

}

void SeekFoodState::OnEnter(Blackboard* pBlackBoard)
{
	AgarioAgent* pAgent;
	AgarioFood* pFood;

	// error checking
	bool isValid = pBlackBoard->GetData("Agent", pAgent);
	if (isValid == false || pAgent == nullptr)
	{
		return;
	}

	isValid = pBlackBoard->GetData("FoodNearByPtr", pFood);
	if (isValid == false || pFood == nullptr)
	{
		return;
	}


	pAgent->SetToSeek( pFood->GetPosition() );
}

// Searching food
bool FSMConditions::FoodNearBYCondition::Evaluate(Blackboard* pBlackboard) const
{
	AgarioAgent* pAgent;
	std::vector<AgarioFood*>* pFoodVec;


	// error checking
	bool isValid = pBlackboard->GetData("Agent", pAgent);
	if (isValid == false || pAgent == nullptr)
	{
		return false;
	}


	// error checking
	isValid = pBlackboard->GetData("FoodVecPtr", pFoodVec);
	if (isValid == false || pFoodVec == nullptr)
	{
		return false;
	}


	// With lambda --> am very terified of this
	const float foodRadius{ pAgent->GetRadius() + 5.0f };
	Vector2 agentPos{ pAgent->GetPosition() };

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawCircle(agentPos, foodRadius, Color{ 1.0f, 0.0f, 0.0f, 1.0f }, DEBUGRENDERER2D->NextDepthSlice());
	}


	auto elementDist = [agentPos](AgarioFood* pFoodElement1, AgarioFood* pFoodElement2) 
	{ 
		float dist1 = agentPos.DistanceSquared(pFoodElement1->GetPosition()); 
		float dist2 = agentPos.DistanceSquared(pFoodElement2->GetPosition());

		return dist1 < dist2;
	};



	auto closestFoodIt = std::min_element(pFoodVec->begin(), pFoodVec->end(), elementDist);
	if (closestFoodIt != pFoodVec->end())
	{
		AgarioFood* pFood = *closestFoodIt;

		if (agentPos.DistanceSquared(pFood->GetPosition()) < (foodRadius * foodRadius))
		{
			pBlackboard->ChangeData("FoodNearByPtr", pFood);
			return true;
		}
	}


	return false;
}

// No food to be found
bool FSMConditions::NoFoodNearBYCondition::Evaluate(Elite::Blackboard* pBlackboard) const
{
	AgarioAgent* pAgent;
	std::vector<AgarioFood*>* pFoodVec;
	AgarioFood* pFood;


	// error checking
	bool isValid = pBlackboard->GetData("Agent", pAgent);
	if (isValid == false || pAgent == nullptr)
	{
		return false;
	}


	// error checking
	isValid = pBlackboard->GetData("FoodVecPtr", pFoodVec);
	if (isValid == false || pFoodVec == nullptr)
	{
		return false;
	}


	// error checking
	isValid = pBlackboard->GetData("FoodNearByPtr", pFood);
	if (isValid == false || pFood == nullptr)
	{
		return false;
	}



	// With lambda --> am very terified of this
	Vector2 agentPos{ pAgent->GetPosition() };

	auto elementDist = [agentPos](AgarioFood* pFoodElement1, AgarioFood* pFoodElement2)
	{
		float dist1 = agentPos.DistanceSquared(pFoodElement1->GetPosition());
		float dist2 = agentPos.DistanceSquared(pFoodElement2->GetPosition());

		return dist1 < dist2;
	};


	auto closestFoodIt = std::find(pFoodVec->begin(), pFoodVec->end(), pFood);
	if (closestFoodIt == pFoodVec->end())
	{
		return true;

	}
}
