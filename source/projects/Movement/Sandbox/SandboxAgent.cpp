#include "stdafx.h"
#include "SandboxAgent.h"

using namespace Elite;

SandboxAgent::SandboxAgent(): BaseAgent()
{
	m_Target = GetPosition();
}

void SandboxAgent::Update(float dt)
{
	const float maxSpeed{ 50.0f };
	const float arrivalRadius{ 1.0f };
	const float slowRadius{ 15.0f };

	// 1 point to another
	Elite::Vector2 toTarget{ m_Target - GetPosition() };

	// If close enough, stop moving
	float distanceSquared{ toTarget.Magnitude() };
	if (distanceSquared < arrivalRadius * arrivalRadius)
	{
		SetLinearVelocity({ 0.0f, 0.0f });
		return;
	}


	// Set velocity
	auto velocity{ toTarget };
	velocity.Normalize();
	
	// Slow down when too fast
	if (distanceSquared < slowRadius * slowRadius)
	{
		velocity *= maxSpeed * distanceSquared / (slowRadius * slowRadius);
	}
	else
	{
		velocity *= maxSpeed;
	}

	SetLinearVelocity(velocity);





	//Orientation
	AutoOrient();
}

void SandboxAgent::Render(float dt)
{
	BaseAgent::Render(dt); //Default Agent Rendering
}

void SandboxAgent::AutoOrient()
{
	//Determine angle based on direction
	Vector2 velocity = GetLinearVelocity();
	if (velocity.Magnitude() > 0)
	{
		velocity.Normalize();
		SetRotation(atan2(velocity.y, velocity.x) + E_PI_2);
	}

	SetRotation(GetRotation() + E_PI_2);
}