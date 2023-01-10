#pragma once


class FlowFieldObstacle
{
public:
	
	FlowFieldObstacle(Elite::Vector2 center, Elite::Vector2 dimensions);
	~FlowFieldObstacle();

	// Move and copy
	FlowFieldObstacle(const FlowFieldObstacle& FlowFieldObstacle) = delete;
	FlowFieldObstacle(FlowFieldObstacle&& FlowFieldObstacle) = delete;
	FlowFieldObstacle& operator=(const FlowFieldObstacle& FlowFieldObstacle) = delete;
	FlowFieldObstacle& operator=(FlowFieldObstacle&& FlowFieldObstacle) = delete;



	Elite::Vector2 GetCenter() const { return m_Center; }
	Elite::Vector2 GetRadius() const { return m_Dimensions; }


private:

	RigidBody* m_pRigidBody{ nullptr };

	Elite::Vector2 m_Center;
	Elite::Vector2 m_Dimensions;
};

