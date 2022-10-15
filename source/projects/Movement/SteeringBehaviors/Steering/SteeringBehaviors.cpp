//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
//Includes
#include "SteeringBehaviors.h"
#include "../SteeringAgent.h"
#include "../Obstacle.h"
#include "framework\EliteMath\EMatrix2x3.h"


//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};

	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();// Direction that is wanted
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();// Rescale to max speed


	if (pAgent->CanRenderBehavior())// draws green line
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0.0f, 1.0f, 0.0f, 1.0f });
	}


	return steering;
}


//FLEE
//****
SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};

	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= -pAgent->GetMaxLinearSpeed();


	// If far enough stop fleeing
	float distance{ m_Target.Position.Distance(pAgent->GetPosition()) };
	if (distance > 20.0f)
	{
		steering.LinearVelocity = Elite::Vector2{ 0.0f, 0.0f };
	}


	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0.0f, 1.0f, 0.0f, 1.0f });
	}


	return steering;
}


//ARRIVE
//****
SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};

	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();


	// Calculation Variables
	float distance{ m_Target.Position.Distance(pAgent->GetPosition()) };
	const float maxSpeed{ 20.0f };
	const float slowRadius{ 20.0f };

	// Slow down when close to target
	if (distance < slowRadius * slowRadius)
	{
		steering.LinearVelocity *= maxSpeed * distance / (slowRadius * slowRadius);
	}
	else
	{
		steering.LinearVelocity *= maxSpeed;
	}


	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0.0f, 1.0f, 0.0f, 1.0f });
	}


	return steering;
}


//FACE
//****
SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};

	Elite::Vector2 direction{ m_Target.Position - pAgent->GetPosition() };// Difference between agent and target
	direction.Normalize();
	float angleBetween{ atan2f(direction.y, direction.x) };// Angle

	steering.AngularVelocity = angleBetween - pAgent->GetRotation();
	pAgent->SetAutoOrient(false);


	// See green line
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), direction, 5, { 0.0f, 1.0f, 0.0f, 1.0f });
		DEBUGRENDERER2D->DrawString(pAgent->GetPosition() + Elite::Vector2(1.5f, 1.5f), std::to_string(Elite::ToDegrees(angleBetween)).c_str());// Shows angle needed to change
	}


	return steering;
}


//WANDER
//****
SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};


	Elite::Vector2 offsetPoint{ pAgent->GetPosition() + (m_OffsetDistance * pAgent->GetDirection()) };


	m_WanderAngle += Elite::randomFloat(-m_MaxAngleChange, m_MaxAngleChange);
	Elite::Vector2 wanderTarget{ (cosf(m_WanderAngle) * m_Radius) + offsetPoint.x, (sinf(m_WanderAngle) * m_Radius) + offsetPoint.y };



	m_Target = wanderTarget;
	if (pAgent->CanRenderBehavior())// draws green line
	{
		// Draw circle
		DEBUGRENDERER2D->DrawCircle(offsetPoint, m_Radius, { 0.0f, 0.0f, 1.0f, 1.0f }, 0.0f);
		DEBUGRENDERER2D->DrawPoint(offsetPoint, 3.0f, { 0.0f, 0.0f, 1.0f, 1.0f });
		DEBUGRENDERER2D->DrawPoint(wanderTarget, 3.0f, { 0.0f, 0.0f, 0.3f, 1.0f });
	
		// Draw direction line
		//DEBUGRENDERER2D->DrawString(pAgent->GetPosition() + Elite::Vector2(1.5f, 1.5f), std::to_string(Elite::ToDegrees(m_WanderAngle)).c_str());// Shows angle needed to change
		DEBUGRENDERER2D->DrawSegment(pAgent->GetPosition(), wanderTarget, Elite::Color{ 0.0f, 1.0f, 0.0f, 1.0f });
	}



	return Seek::CalculateSteering(deltaT, pAgent);
}


void Wander::SetWanderOffset(float offset)
{
	m_OffsetDistance = offset;
}

void Wander::SetWanderRadius(float radius)
{
	m_Radius = radius;
}

void Wander::SetMaxAngleChange(float rad)
{
	m_MaxAngleChange = rad;
}


//PURSUIT
//****
SteeringOutput Pursuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	float futurePath{ 2.0f };


	steering.LinearVelocity = (m_Target.Position + (m_Target.LinearVelocity * futurePath)) - pAgent->GetPosition();// Direction towards other agents target
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();// Rescale to max speed


	if (pAgent->CanRenderBehavior())// draws green line - does not work!
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0.0f, 1.0f, 0.0f, 1.0f });
	}


	return steering;
}


//EVADE
//****
SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	float futurePath{ 3.0f };
	float distance{ m_Target.Position.Distance(pAgent->GetPosition()) };


	if (distance < m_EvadeRadius)// If not far enough start fleeing
	{
		steering.LinearVelocity = (m_Target.Position + (m_Target.LinearVelocity * futurePath)) - pAgent->GetPosition();// Direction towards other agents target
		steering.LinearVelocity.Normalize();
		steering.LinearVelocity *= -pAgent->GetMaxLinearSpeed();// Rescale to negative max speed

		steering.IsValid = true;
	}
	else // If far enough stop fleeing
	{
		steering.LinearVelocity = Elite::Vector2{ 0.0f, 0.0f };
		steering.IsValid = false;
	}


	if (pAgent->CanRenderBehavior())// draws green line - does not work!
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0.0f, 1.0f, 0.0f, 1.0f });
	}
	

	return steering;
}

