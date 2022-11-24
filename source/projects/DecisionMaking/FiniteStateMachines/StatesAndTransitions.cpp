#include "stdafx.h"
#include "StatesAndTransitions.h"

using namespace Elite;
using namespace FSMState;


void WanderState::OnEnter(Blackboard* pBlackBoard)
{
	AgarioAgent* pAgent;
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
	bool isValid = pBlackBoard->GetData("Agent", pAgent);

	if (isValid == false || pAgent == nullptr)
	{
		return;
	}
		
	
	AgarioFood* pFood;
	isValid = pBlackBoard->GetData("FoodNearByPtr", pFood);

	if (isValid == false || pFood == nullptr)
	{
		return;
	}


	pAgent->SetToSeek(pFood->GetPosition());
}


void EvadeState::OnEnter(Blackboard* pBlackBoard)
{
	AgarioAgent* pAgent;
	bool isValid = pBlackBoard->GetData("Agent", pAgent);

	if (isValid == false || pAgent == nullptr)
	{
		return;
	}


	pAgent->SetToWander();
}

void EvadeState::Update(Blackboard* pBlackboard, float deltaTime)
{
	AgarioAgent* pAgent;
	bool isValid = pBlackboard->GetData("Agent", pAgent);

	if (isValid == false || pAgent == nullptr)
		return;


	AgarioAgent* pEvadeAgent;
	isValid = pBlackboard->GetData("BigAgent", pEvadeAgent);

	if (isValid == false || pEvadeAgent == nullptr)
		return;


	pAgent->SetToFlee(pEvadeAgent->GetPosition());
}


void PursueState::OnEnter(Elite::Blackboard* pBlackBoard)
{
	AgarioAgent* pAgent;
	bool isValid = pBlackBoard->GetData("Agent", pAgent);

	if (isValid == false || pAgent == nullptr)
	{
		return;
	}


	pAgent->SetToWander();
}

void PursueState::Update(Blackboard* pBlackboard, float deltaTime)
{
	AgarioAgent* pAgent;
	bool isValid = pBlackboard->GetData("Agent", pAgent);

	if (isValid == false || pAgent == nullptr)
		return;


	AgarioAgent* pPursueAgent;
	isValid = pBlackboard->GetData("SmallAgent", pPursueAgent);

	if (isValid == false || pPursueAgent == nullptr)
		return;


	pAgent->SetToSeek(pPursueAgent->GetPosition());
}



// Searching food
bool FSMConditions::FoodNearByCondition::Evaluate(Blackboard* pBlackboard) const
{
	AgarioAgent* pAgent;
	bool isValid = pBlackboard->GetData("Agent", pAgent);
	
	if (isValid == false || pAgent == nullptr)
	{
		return false;
	}


	std::vector<AgarioFood*>* pFoodVec;
	isValid = pBlackboard->GetData("FoodVecPtr", pFoodVec);
	
	if (isValid == false || pFoodVec == nullptr)
	{
		return false;
	}



	// With lambda --> am very terified of this
	const float foodRadius{ pAgent->GetRadius() + 10.0f };
	Vector2 agentPos{ pAgent->GetPosition() };

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
bool FSMConditions::NoFoodNearByCondition::Evaluate(Blackboard* pBlackboard) const
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

	// If no food
	auto closestFoodIt = std::find(pFoodVec->begin(), pFoodVec->end(), pFood);
	if (closestFoodIt == pFoodVec->end())
	{
		return true;
	}
}


// Bigger agent nearby -- this was pure pain
bool FSMConditions::EvadeAgentCondition::Evaluate(Blackboard* pBlackboard) const
{
	AgarioAgent* pAgent;
	bool isValid = pBlackboard->GetData("Agent", pAgent);

	if (isValid == false || pAgent == nullptr)
		return false;


	std::vector<AgarioAgent*>* pAgentsVec;
	isValid = pBlackboard->GetData("AgentVecPtr", pAgentsVec);

	if (isValid == false || pAgentsVec == nullptr)
		return false;


	// find the closest agents
	const float evadeRadius{ pAgent->GetRadius() + 20.0f };
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

		if (agentPos.DistanceSquared(pBiggerAgernt->GetPosition()) <= (evadeRadius * evadeRadius))
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

// No bigger agent nearby
bool FSMConditions::NoBiggerAgentNearByCondition::Evaluate(Blackboard* pBlackboard) const
{
	AgarioAgent* pAgent;
	std::vector<AgarioAgent*>* pAgentsVec;
	AgarioAgent* pBiggerAgent;

	// error checking
	bool isValid = pBlackboard->GetData("Agent", pAgent);
	if (isValid == false || pAgent == nullptr)
		return false;

	// error checking
	isValid = pBlackboard->GetData("AgentVecPtr", pAgentsVec);
	if (isValid == false || pAgentsVec == nullptr)
		return false;

	// error checking
	isValid = pBlackboard->GetData("BigAgent", pBiggerAgent);
	if (isValid == false || pBiggerAgent == nullptr)
		return false;


	// if no bigger agent
	const float evadeRadius{ pAgent->GetRadius() + 15.0f };
	const Vector2 agentPos{ pAgent->GetPosition() };

	auto closestAgentIt = std::find(pAgentsVec->begin(), pAgentsVec->end(), pBiggerAgent);

	if (closestAgentIt != pAgentsVec->end())
	{
		AgarioAgent* pBiggerAgernt = *closestAgentIt;

		if (agentPos.DistanceSquared(pBiggerAgernt->GetPosition()) >= (evadeRadius * evadeRadius))
		{
			if (pAgent->GetRadius() < pBiggerAgernt->GetRadius())
			{
				return true;
			}

		}
	}


}


// Smaller agent nearby
bool FSMConditions::SmallerAgentNearByCondition::Evaluate(Elite::Blackboard* pBlackboard) const
{
	AgarioAgent* pAgent;
	bool isValid = pBlackboard->GetData("Agent", pAgent);

	if (isValid == false || pAgent == nullptr)
		return false;


	std::vector<AgarioAgent*>* pAgentsVec;
	isValid = pBlackboard->GetData("AgentVecPtr", pAgentsVec);

	if (isValid == false || pAgentsVec == nullptr)
		return false;


	// find the closest agents
	const float PursueRadius{ pAgent->GetRadius() + 10.0f };
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
		AgarioAgent* pSmallerAgernt = *closestAgentIt;

		if (agentPos.DistanceSquared(pSmallerAgernt->GetPosition()) <= (PursueRadius * PursueRadius))
		{
			if (pAgent->GetRadius() > pSmallerAgernt->GetRadius())
			{
				pBlackboard->ChangeData("SmallAgent", pSmallerAgernt);
				return true;
			}

		}
	}


	return false;
}

bool FSMConditions::NoSmallerAgentNearByCondition::Evaluate(Elite::Blackboard* pBlackboard) const
{
	AgarioAgent* pAgent;
	std::vector<AgarioAgent*>* pAgentsVec;
	AgarioAgent* pSmallAgent;


	// error checking
	bool isValid = pBlackboard->GetData("Agent", pAgent);
	if (isValid == false || pAgent == nullptr)
	{
		return false;
	}


	// error checking
	isValid = pBlackboard->GetData("AgentVecPtr", pAgentsVec);
	if (isValid == false || pAgentsVec == nullptr)
	{
		return false;
	}


	// error checking
	isValid = pBlackboard->GetData("SmallAgent", pSmallAgent);
	if (isValid == false || pSmallAgent == nullptr)
	{
		return false;
	}


	// If no smaller agent
	auto closestFoodIt = std::find(pAgentsVec->begin(), pAgentsVec->end(), pSmallAgent);
	if (closestFoodIt == pAgentsVec->end())
	{
		return true;
	}
}


