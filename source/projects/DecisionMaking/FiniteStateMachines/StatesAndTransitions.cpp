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

void EvadeState::OnEnter(Blackboard* pBlackBoard)
{
	AgarioAgent* pAgent;
	AgarioAgent* pEvadeAgent;
	float* fleeRadius;


	// error checking
	bool isValid = pBlackBoard->GetData("Agent", pAgent);
	if (isValid == false || pAgent == nullptr)
		return;

	isValid = pBlackBoard->GetData("BigAgent", pEvadeAgent);
	if (isValid == false || pEvadeAgent == nullptr)
		return;
	

	pAgent->SetToFlee( pEvadeAgent->GetPosition() );
}


// Searching food
bool FSMConditions::FoodNearByCondition::Evaluate(Blackboard* pBlackboard) const
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
bool FSMConditions::NoFoodNearByCondition::Evaluate(Elite::Blackboard* pBlackboard) const
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

	auto closestFoodIt = std::find(pFoodVec->begin(), pFoodVec->end(), pFood);
	if (closestFoodIt == pFoodVec->end())
	{
		return true;
	}
}


// Bigger agent nearby -- this was pure pain
bool FSMConditions::EvadeAgentCondition::Evaluate(Elite::Blackboard* pBlackboard) const
{
	AgarioAgent* pAgent;
	std::vector<AgarioAgent*>* pAgentsVec;


	// error checking
	bool isValid = pBlackboard->GetData("Agent", pAgent);
	if (isValid == false || pAgent == nullptr)
		return false;
	
	// error checking
	isValid = pBlackboard->GetData("EvadeAgentVecPtr", pAgentsVec);
	if (isValid == false || pAgentsVec == nullptr)
		return false;
	

	// find the closest agents
	const float evadeRadius{ pAgent->GetRadius() + 15.0f };
	const Vector2 agentPos{ pAgent->GetPosition() };



	// With lambda --> am very terified of this
	auto elementDist = [agentPos](AgarioAgent* pAgent1, AgarioAgent* pAgent2)
	{
		float dist1 = agentPos.DistanceSquared(pAgent1->GetPosition());
		float dist2 = agentPos.DistanceSquared(pAgent2->GetPosition());

		return dist1 < dist2;
	};


	auto closestAgentIt = std::min_element(pAgentsVec->begin(), pAgentsVec->end(), elementDist);


	if (closestAgentIt != pAgentsVec->end())
	{
		AgarioAgent* pBiggerAgernt = *closestAgentIt;

		if (agentPos.DistanceSquared(pBiggerAgernt->GetPosition() ) <= (evadeRadius * evadeRadius))
		{
			if (pAgent->GetRadius() < pBiggerAgernt->GetRadius())
			{
				pBlackboard->ChangeData("BigAgent", pBiggerAgernt);
				return true;
			}

		}
	}


	return false;
}

// if no bigger 
bool FSMConditions::NoBiggerAgentNearByCondition::Evaluate(Elite::Blackboard* pBlackboard) const
{
	AgarioAgent* pAgent;
	std::vector<AgarioAgent*>* pAgentsVec;
	AgarioAgent* pBiggerAgent;

	// error checking
	bool isValid = pBlackboard->GetData("Agent", pAgent);
	if (isValid == false || pAgent == nullptr)
		return false;

	// error checking
	isValid = pBlackboard->GetData("EvadeAgentVecPtr", pAgentsVec);
	if (isValid == false || pAgentsVec == nullptr)
		return false;

	// error checking
	isValid = pBlackboard->GetData("BigAgent", pBiggerAgent);
	if (isValid == false || pBiggerAgent == nullptr)
		return false;
	

	// find the closest agents
	const Vector2 agentPos{ pAgent->GetPosition() };

	// With lambda --> am very terified of this
	auto closestAgentIt = std::find(pAgentsVec->begin(), pAgentsVec->end(), pBiggerAgent);
	if (closestAgentIt == pAgentsVec->end())
	{
		return true;
	}
}
