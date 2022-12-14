/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// Behaviors.h: Implementation of certain reusable behaviors for the BT version of the Agario Game
/*=============================================================================*/
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteMath/EMath.h"
#include "framework/EliteAI/EliteDecisionMaking/EliteBehaviorTree/EBehaviorTree.h"
#include "projects/Shared/Agario/AgarioAgent.h"
#include "projects/Shared/Agario/AgarioFood.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------

namespace BHT_Actions
{
	Elite::BehaviorState ChangeToWander(Elite::Blackboard* pBlackBoard)
	{
		AgarioAgent* pAgent;

		if (!pBlackBoard->GetData("Agent", pAgent) || pAgent == nullptr)
			return Elite::BehaviorState::Failure;


		pAgent->SetToWander();
		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState ChangeToSeek(Elite::Blackboard* pBlackBoard)
	{
		AgarioAgent* pAgent;
		Elite::Vector2 target;

		if (!pBlackBoard->GetData("Agent", pAgent) || pAgent == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		if (!pBlackBoard->GetData("Target", target))
		{
			return Elite::BehaviorState::Failure;
		}


		pAgent->SetToSeek(target);
		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState ChangeToEvade(Elite::Blackboard* pBlackBoard)
	{
		AgarioAgent* pAgent;
		AgarioAgent* pBiggerAgent;

		if (!pBlackBoard->GetData("Agent", pAgent) || pAgent == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		if (!pBlackBoard->GetData("AgentFleeTarget", pBiggerAgent) || pBiggerAgent == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}


		pAgent->SetToEvade(pBiggerAgent->GetPosition());
		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState ChangeToPursuit(Elite::Blackboard* pBlackBoard)
	{
		AgarioAgent* pAgent;
		Elite::Vector2 target;

		if (!pBlackBoard->GetData("Agent", pAgent) || pAgent == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		if (!pBlackBoard->GetData("Target", target))
		{
			return Elite::BehaviorState::Failure;
		}

		pAgent->SetToPursuit(target);
		return Elite::BehaviorState::Success;
	}
}

namespace BHT_Conditions
{
	bool IsFoodNearby(Elite::Blackboard* pBlackBoard)
	{
		AgarioAgent* pAgent;
		std::vector<AgarioFood*>* pFoodVec;


		if (!pBlackBoard->GetData("Agent", pAgent) || pAgent == nullptr)
		{
			return false;
		}

		if (!pBlackBoard->GetData("FoodVec", pFoodVec) || pFoodVec == nullptr)
		{
			return false;
		}


		// Gettign the closest food
		const float searchRadius{ pAgent->GetRadius() + 10.0f };
		Elite::Vector2 agentPos{ pAgent->GetPosition() };
		AgarioFood* pClosestFood = nullptr;
		float closestDistSq{ searchRadius * searchRadius };

		for (auto& pFood : *pFoodVec)
		{
			float distSq{ pFood->GetPosition().DistanceSquared(agentPos) };

			if (distSq < closestDistSq)
			{
				pClosestFood = pFood;
				closestDistSq = distSq;
			}
		}

		if (pClosestFood != nullptr)
		{
			pBlackBoard->ChangeData("Target", pClosestFood->GetPosition());
			return true;
		}

		return false;
	}

	bool IsBiggerAgentNearby(Elite::Blackboard* pBlackBoard)
	{
		AgarioAgent* pAgent;
		std::vector<AgarioAgent*>* pAgentsVec;


		if (!pBlackBoard->GetData("Agent", pAgent) || pAgent == nullptr)
		{
			return false;
		}

		if (!pBlackBoard->GetData("AgentsVec", pAgentsVec) || pAgentsVec == nullptr)
		{
			return false;
		}


		// Gettign the closest food
		const float evadeRadius{ pAgent->GetRadius() + 20.0f };
		Elite::Vector2 agentPos{ pAgent->GetPosition() };
		AgarioAgent* pClosestAgent = nullptr;
		float closestDistSq{ evadeRadius * evadeRadius };

		for (auto& pAgent : *pAgentsVec)
		{
			float distSq{ pAgent->GetPosition().DistanceSquared(agentPos) };

			if (distSq < closestDistSq)
			{
				pClosestAgent = pAgent;
				closestDistSq = distSq;
			}
		}

		if (pClosestAgent != nullptr)
		{
			if (pAgent->GetRadius() < pClosestAgent->GetRadius())
			{
				pBlackBoard->ChangeData("AgentFleeTarget", pClosestAgent);
				return true;
			}
		}

		return false;
	}

	bool IsSmallerAgentNearby(Elite::Blackboard* pBlackBoard)
	{
		AgarioAgent* pAgent;
		std::vector<AgarioAgent*>* pAgentsVec;


		if (!pBlackBoard->GetData("Agent", pAgent) || pAgent == nullptr)
		{
			return false;
		}

		if (!pBlackBoard->GetData("AgentsVec", pAgentsVec) || pAgentsVec == nullptr)
		{
			return false;
		}


		// Gettign the closest food
		const float evadeRadius{ pAgent->GetRadius() + 10.0f };
		Elite::Vector2 agentPos{ pAgent->GetPosition() };
		AgarioAgent* pClosestAgent = nullptr;
		float closestDistSq{ evadeRadius * evadeRadius };

		for (auto& pAgent : *pAgentsVec)
		{
			float distSq{ pAgent->GetPosition().DistanceSquared(agentPos) };

			if (distSq < closestDistSq)
			{
				pClosestAgent = pAgent;
				closestDistSq = distSq;
			}
		}

		if (pClosestAgent != nullptr)
		{
			if (pAgent->GetRadius() > (pClosestAgent->GetRadius() + 1.0f))
			{
				pBlackBoard->ChangeData("Target", pClosestAgent->GetPosition());
				return true;
			}
		}

		return false;




	}


	Elite::InfluenceNode* GetHighestInfluencedNode(Elite::Blackboard* pBlackBoard)
	{
		AgarioAgent* pAgent;
		if (!pBlackBoard->GetData("Agent", pAgent) || pAgent == nullptr)
		{
			return false;
		}

		Elite::InfluenceMap<Elite::GridGraph<Elite::InfluenceNode, Elite::GraphConnection>>* pInfluenceGrid{ nullptr };
		if (!pBlackBoard->GetData("InfluenceGrid", pInfluenceGrid) || pInfluenceGrid == nullptr)
		{
			return false;
		}


		// Gettign the highest influenced node
		const float maxRange{ pAgent->GetRadius() + 10.0f };
		Elite::InfluenceNode* pHighestInfluencedNode{ nullptr };

		for (auto& pNode : pInfluenceGrid->GetAllNodes())
		{
			if ((pNode->GetInfluence() > pHighestInfluencedNode->GetInfluence()) &&
				(DistanceSquared(pInfluenceGrid->GetNodeWorldPos(pNode), pAgent->GetPosition()) < (maxRange * maxRange))
				)
			{
				pHighestInfluencedNode = pNode;
				continue;
			}
		}


		return pHighestInfluencedNode;
	}

	Elite::BehaviorState InfluencedAgent(Elite::Blackboard* pBlackBoard)
	{
		AgarioAgent* pAgent;
		if (!pBlackBoard->GetData("Agent", pAgent) || pAgent == nullptr)
		{
			return  Elite::BehaviorState::Failure;
		}

		Elite::InfluenceMap<Elite::GridGraph<Elite::InfluenceNode, Elite::GraphConnection>>* pInfluenceGrid{ nullptr };
		if (!pBlackBoard->GetData("InfluenceGrid", pInfluenceGrid) || pInfluenceGrid == nullptr)
		{
			return  Elite::BehaviorState::Failure;
		}


		pInfluenceGrid->SetInfluenceAtPosition
		(
			pAgent->GetPosition(),
			7.5f + pInfluenceGrid->GetNodeAtWorldPos(pAgent->GetPosition())->GetInfluence()
		);
		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState GoToHighestInfluence(Elite::Blackboard* pBlackBoard)
	{
		AgarioAgent* pAgent;
		if (!pBlackBoard->GetData("Agent", pAgent) || pAgent == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		Elite::InfluenceMap<Elite::GridGraph<Elite::InfluenceNode, Elite::GraphConnection>>* pInfluenceGrid{ nullptr };
		if (!pBlackBoard->GetData("InfluenceGrid", pInfluenceGrid) || pInfluenceGrid == nullptr)
		{
			return  Elite::BehaviorState::Failure;
		}


		pBlackBoard->ChangeData("Target", pInfluenceGrid->GetNodeWorldPos(GetHighestInfluencedNode(pBlackBoard) ) );
		return Elite::BehaviorState::Success;
	}
}
#endif