#include "stdafx.h"
#include "FlowFieldObstacle.h"


FlowFieldObstacle::FlowFieldObstacle(Elite::Vector2 center, Elite::Vector2 dimensions)
{
	//Create Rigidbody
	const Elite::RigidBodyDefine define = Elite::RigidBodyDefine(0.01f, 0.1f, Elite::eStatic, false);
	const Transform transform = Transform(center, Elite::ZeroVector2);
	m_pRigidBody = new RigidBody(define, transform);

	//Add shape
	Elite::EPhysicsBoxShape shape;// Look out this is box shape

	shape.height = dimensions.y;
	shape.width = dimensions.x;
	m_pRigidBody->AddShape(&shape);
}


FlowFieldObstacle::~FlowFieldObstacle()
{
	SAFE_DELETE(m_pRigidBody);
}