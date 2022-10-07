#include "stdafx.h"
#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"


//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};


	steering.LinearVelocity = Elite::Vector2{
												m_pFlock->GetAverageNeighborPos().x - pAgent->GetPosition().x,
												m_pFlock->GetAverageNeighborPos().y - pAgent->GetPosition().y
											};
	
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();// Rescale to max speed


	return steering;
}


//*********************
//SEPARATION (FLOCKING)
SteeringOutput Separation::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};


	// go trough every neighbor and adjust the length
	for(int idx{}; idx < m_pFlock->GetNrOfNeighbors(); ++idx)
	{
		// calculate distance
		Elite::Vector2 distance{ pAgent->GetPosition() - m_pFlock->GetNeighbors()[idx]->GetPosition()};

		// Distance / by it's magnitude squared
		steering.LinearVelocity +=  distance / distance.MagnitudeSquared() ;
	}
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();// Rescale to max speed


	return steering;
}


//*************************
//VELOCITY MATCH (FLOCKING)
SteeringOutput VelocityMatch::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};


	steering.LinearVelocity = m_pFlock->GetAverageNeighborVelocity();
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= -pAgent->GetMaxLinearSpeed();// Rescale to max speed



	return steering;
}