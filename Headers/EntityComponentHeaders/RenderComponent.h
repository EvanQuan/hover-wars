#pragma once
#include "stdafx.h"
#include "EntityComponent.h"

/************************************************************
* Name: RenderComponent
* Written by: James Cote
* Description: Component used for storing vertex information and handles
*	for the GPU to render to the screen.
***************************************************************/
class RenderComponent 
	: public EntityComponent
{
public:
	RenderComponent(int iEntityID, int iComponentID);
	virtual ~RenderComponent();

	void update(void);

private: 
	// Private Copy Constructor and Assignment operator overload.
	RenderComponent(const RenderComponent* pCopy);
	RenderComponent& operator=(const RenderComponent& pRHS);

	
};
