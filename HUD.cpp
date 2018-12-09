#include "HUD.h"
#include "Game.h"


HUD::HUD(float health, float O2Levels)
{
	m_health = health;
	m_O2 = O2Levels;


	m_pD3DDevice = TheGame::Singleton()->getDevice();
	m_pImmediateContext = TheGame::Singleton()->getContext();

	text = new Text2D("assets/fonts/font1.bmp", m_pD3DDevice, m_pImmediateContext);
}


HUD::~HUD()
{
}

bool HUD::Init(){

	m_pD3DDevice = TheGame::Singleton()->getDevice();
	m_pImmediateContext = TheGame::Singleton()->getContext();

	m_health = 100.0f;
	m_O2 = 100.0f;

	text = new Text2D("assets/fonts/font1.bmp", m_pD3DDevice, m_pImmediateContext);

	return true;

}


void HUD::setHealth(float amount) {

	m_health -= amount;

}

void HUD::setO2(float amount) { 

	if (m_O2 - amount < 0){

		m_O2 = 0; 
	}

	m_O2 -= amount;

}

void HUD::addO2(float amount) {

	m_O2 += amount; 

}

void HUD::displayHealth() {

	text->AddText("Health: ", -1.0, +1.0, 0.2); 

}

void HUD::displayO2() {

	text->AddText("02: ", -1.0, +1.0, 0.2);

}

