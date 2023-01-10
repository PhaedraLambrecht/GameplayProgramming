#pragma once


class Obstacle
{
public:
	
	Obstacle(Elite::Vector2 center, Elite::Vector2 dimensions);
	~Obstacle();

	// Move and copy
	Obstacle(const Obstacle& Obstacle) = delete;
	Obstacle(Obstacle&& Obstacle) = delete;
	Obstacle& operator=(const Obstacle& Obstacle) = delete;
	Obstacle& operator=(Obstacle&& Obstacle) = delete;



	Elite::Vector2 GetCenter() const { return m_Center; }
	Elite::Vector2 GetRadius() const { return m_Dimensions; }


private:

	RigidBody* m_pRigidBody{ nullptr };

	Elite::Vector2 m_Center;
	Elite::Vector2 m_Dimensions;
};

