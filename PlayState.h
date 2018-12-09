#pragma once
#include <vector>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include "GameState.h"
#include "Model.h"
#include "SceneNode.h"
#include "Camera.h"
#include "SkyBox.h"
#include "Input.h"
#include "text2D.h"
#include "HUD.h"
#include "PickUp.h"


class PlayState : public GameState
{
public:
	PlayState(); 

	virtual void update();
	virtual void render(); 
	virtual bool onEnter();
	virtual bool onExit(); 
	virtual std::string getStateID() const { return s_playID; }

	void ListenForKeys(); 

	bool draw;
	bool hasQuestItem;

private:
	static const std::string s_playID;

	ID3D11Device*				g_pD3DDevice = NULL;
	ID3D11DeviceContext*		g_pImmediateContext = NULL;
	IDXGISwapChain*				g_pSwapChain = NULL;
	ID3D11RenderTargetView*		g_pBackBufferRTView = NULL;
	HWND						g_hWnd = NULL;

	//////////////////////////////////////////////////////////////////////////////////////
	//							WORLD
	//////////////////////////////////////////////////////////////////////////////////////
	XMMATRIX world, projection, view;
	XMMATRIX transpose;

	//////////////////////////////////////////////////////////////////////////////////////
	//							Lighting Varibales
	//////////////////////////////////////////////////////////////////////////////////////
	XMVECTOR					g_directional_light_shines_from;
	XMVECTOR					g_directional_light_colour;
	XMVECTOR					g_ambient_light_colour;

	//////////////////////////////////////////////////////////////////////////////////////
	//							Input Variables
	//////////////////////////////////////////////////////////////////////////////////////
	IDirectInput8*				g_direct_input;
	IDirectInputDevice8*		g_keyboard_device;
	unsigned char				g_keyboard_keys_state[256];

	///////////////////////////////////////////
	// Model
	///////////////////////////////////////////
	Model*						g_ModelCamera;
	Model*						g_ModelFloor;
	Model*						g_ModelRock;
	Model*						g_ModelRock2;
	Model*						g_ModelRock3;
	Model*						g_ModelRock4;
	Model*						g_ModelEarth;
	Model*						g_ModelMoon;
	Model*						g_ModelSun; 
	Model*						g_ModelO2Cannister;
	Model*						g_ModelO2Cannister2;
	Model*						g_ModelO2Cannister3;
	Model*						g_ModelQuestItem;
	Model*						g_ModelQuestItem2;
	Model*						g_ModelQuestItem3; 

	///////////////////////////////////////////
	// Node
	///////////////////////////////////////////
	SceneNode*					g_root_node;
	SceneNode*					g_CameraNode;
	SceneNode*					g_LevelNode;
	SceneNode*					g_RockNode;
	SceneNode*					g_RockNode2;
	SceneNode*					g_RockNode3;
	SceneNode*					g_RockNode4;
	SceneNode*					g_EarthNode;
	SceneNode*					g_MoonNode;
	SceneNode*					g_SunNode;
	SceneNode*					g_O2CannisterNode;
	SceneNode*					g_O2CannisterNode2;
	SceneNode*					g_O2CannisterNode3;
	SceneNode*					g_QuestItemNode;
	SceneNode*					g_QuestItemNode2;
	SceneNode*					g_QuestItemNode3;

	///////////////////////////////////////////
	// Other
	///////////////////////////////////////////
	Camera*						g_GameCamera;
	SkyBox*						g_SkyBox;
	Text2D*						g_2DText;
	HUD*						g_Hud;
	PickUp*						g_PickUp;

};

