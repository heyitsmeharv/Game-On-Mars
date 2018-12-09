#include "PickUp.h"
#include "Game.h"
#include "PlayState.h"

PickUp::PickUp(Model* model, HUD* hud, PlayState* play) : SceneNode(model)
{

	m_pModel = model; 
	m_pHud = hud; 
	m_play = play; 

}


PickUp::~PickUp()
{
}

void PickUp::pickUpO2Object(){

	m_pHud->addO2(0.50f);

	draw = false;
	
}

void PickUp::pickUpQuestObject(){

	m_play->hasQuestItem = true;
}

void PickUp::execute(XMMATRIX *world, XMMATRIX* view, XMMATRIX* projection){


	SceneNode::execute(world, view, projection); 

}