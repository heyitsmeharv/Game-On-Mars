#pragma once
#include "text2D.h"


class HUD
{
public:
	HUD(float health, float O2Levels);
	~HUD();

	float m_health;
	float m_O2;

	float getHealth() { return m_health; }
	float getO2() { return m_O2; }
	void setHealth(float amount);
	void setO2(float amount);
	void addO2(float amount);
	
	void displayHealth();
	void displayO2();

	bool Init(); 

private:


protected:
	//static HUD* s_pInstance;
	

	Text2D*					text; 
	ID3D11Device*			m_pD3DDevice;
	ID3D11DeviceContext*	m_pImmediateContext;

};
//typedef HUD TheHUD;