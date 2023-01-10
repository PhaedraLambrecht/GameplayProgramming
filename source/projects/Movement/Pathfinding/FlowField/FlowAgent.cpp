#include "stdafx.h"
#include "FlowAgent.h"


//Constructor & Destructor
FlowAgent::FlowAgent(const Elite::Vector2& spawnPos)
{
	m_pAgent = new SteeringAgent();
	m_pAgent->SetBodyColor(m_AgentColor);
	m_pAgent->SetPosition(spawnPos);


	m_pBehavior = new Seek();
}

FlowAgent::~FlowAgent()
{
	SAFE_DELETE(m_pAgent);
	SAFE_DELETE(m_pBehavior);
}


// Functions
void FlowAgent::Render(float dt)
{
	m_pAgent->Render(dt);
	
	DrawTarget();
}

void FlowAgent::Update(float dt)
{
	m_HasMadeFirstMove = false;
	
	
	if (m_HasReachedGoal)
	{
		m_pAgent->SetLinearVelocity({ 0,0 });
		
		if (m_CurrentTime < m_MaxWaitTime)
		{
			m_CurrentTime += dt;
		}
		else
		{
			m_NeedsToGo = true;
			m_CurrentTime = m_MaxWaitTime;
		}
	}


	m_pBehavior->SetTarget(m_TargetPos);
	m_pAgent->SetSteeringBehavior(m_pBehavior);
	m_pAgent->Update(dt);
}


Elite::Vector2 FlowAgent::GetPos() const
{
	return m_pAgent->GetPosition();
}

Elite::Vector2& FlowAgent::GetTargetPos()
{
	return m_TargetPos;
}

void FlowAgent::SetHasReachedGoal(bool reachedGoal)
{
	m_HasReachedGoal = reachedGoal;
}

bool FlowAgent::GetHasReachedGoal() const
{
	return m_HasReachedGoal;
}

bool FlowAgent::GetFirstMove() const
{
	return m_HasMadeFirstMove;
}

bool FlowAgent::GetNeedsToGo() const
{
	return m_NeedsToGo;
}


//-------------------
// Helper functions
//-------------------
void FlowAgent::DrawTarget() const
{
	DEBUGRENDERER2D->DrawCircle(m_TargetPos, 2.0f, { 1.0f, 1.0f, 1.0f }, DEBUGRENDERER2D->NextDepthSlice());
}


//void FlowAgent::TrimToWorld(float worldBounds, bool isWorldLooping) 
//{
//	TrimToWorld({ 0, 0 }, { worldBounds, worldBounds }, isWorldLooping);
//}
//void FlowAgent::TrimToWorld(const Elite::Vector2& bottomLeft, const Elite::Vector2& topRight, bool isWorldLooping /* = true */) const
//{
//	auto pos = m_pAgent->GetPosition();
//
//	if (isWorldLooping)
//	{
//		if (pos.x > topRight.x)
//			pos.x = bottomLeft.x;
//		else if (pos.x < bottomLeft.x)
//			pos.x = topRight.x;
//
//		if (pos.y > topRight.y)
//			pos.y = bottomLeft.y;
//		else if (pos.y < bottomLeft.y)
//			pos.y = topRight.y;
//	}
//	else
//	{
//		pos.x = Elite::Clamp(pos.x, bottomLeft.x, topRight.x);
//		pos.y = Elite::Clamp(pos.y, bottomLeft.y, topRight.y);
//	}
//
//	m_pAgent->SetPosition(pos);
//}