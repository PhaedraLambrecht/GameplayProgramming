#ifndef FLOWFIELD_APPLICATION_H
#define FLOWFIELD_APPLICATION_H
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"


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


	bool m_VisualizeTarget = true;
	bool m_TrimWorld = true;
	float m_TrimWorldSize = 50.f;


	// Helper functions
	void UpdateImGui();
};


#endif