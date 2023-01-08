#include "stdafx.h"
#include "FlowAgent.h"


FlowAgent::FlowAgent(const Elite::Vector2& spawnPos)
{
	m_pAgent = new SteeringAgent{};

	m_pAgent->SetPosition(spawnPos);
	m_pAgent->SetBodyColor(m_AgentColor);
}

FlowAgent::~FlowAgent()
{
	SAFE_DELETE(m_pAgent);
}


void FlowAgent::Update(float dt)
{
	m_pAgent->Update(dt);
}

void FlowAgent::Render(float dt)
{
	m_pAgent->Render(dt);
}

