#pragma once
#include "SceneNode.h"
#include "Model.h" 
#include "HUD.h"

class PlayState;

class PickUp : public SceneNode
{
public:
	PickUp(Model* model, HUD* hud, PlayState* play);
	~PickUp();

	void pickUpO2Object();
	void pickUpQuestObject();
	virtual void execute(XMMATRIX *world, XMMATRIX* view, XMMATRIX* projection);

private:

	Model*						m_pModel; 
	HUD*						m_pHud;
	PlayState*					m_play; 

protected:



};

