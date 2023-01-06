#ifndef FLOWFIELD_APPLICATION_H
#define FLOWFIELD_APPLICATION_H
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "Grid.h"


class App_FlowField final : public IApp
{
public:

	//Constructor & Destructor
	App_FlowField() = default;
	virtual ~App_FlowField();


	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;


private:


	//bool m_VisualizeTarget = true;
	bool m_TrimWorld = false;
	float m_TrimWorldSize;


	Grid* m_pGrid;
	bool m_MadeObstacles{ false };
	bool m_MadeGoals{ false };
	bool m_MadeFlowfield{ false };


	// For visualization toggles
	bool m_IsGridDrawn{true};
	bool m_IsObstacleDrawn{ true };
	bool m_IsGoalDrawn{ true };
	bool m_IsDirectionDrawn{ true };



	// Helper functions
	void UpdateImGui();
};


#endif