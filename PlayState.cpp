#include "PlayState.h"
#include "GameOverState.h"
#include "Game.h"
#include "Input.h"
#include <iostream>

const std::string PlayState::s_playID = "PLAY";


PlayState::PlayState(){

	g_pD3DDevice = TheGame::Singleton()->getDevice();
	g_pImmediateContext = TheGame::Singleton()->getContext(); 

	g_Hud = new HUD(100, 100);

	draw = true;
	hasQuestItem = false;

}

void PlayState::render(){



}

void PlayState::update(){
	
	//////////////////////////////////////////////////////
	// Clear the back buffer - choose a colour you like
	//////////////////////////////////////////////////////
	float rgba_clear_colour[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // Black
	g_pImmediateContext->ClearRenderTargetView(g_pBackBufferRTView, rgba_clear_colour);

	///////////////////////////////////////////
	// Lighting
	///////////////////////////////////////////
	g_directional_light_shines_from = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	g_directional_light_colour = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	g_ambient_light_colour = XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f);

	///////////////////////////////////////////
	// World Space
	///////////////////////////////////////////
	world = XMMatrixIdentity(); 
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), 1080.0f / 720.0f, 0.1f, 500000.0f); // fucking zbuffer bullshit. 
	view = g_GameCamera->GetViewMatrix();

	///////////////////////////////////////////
	// Keyboard Input
	///////////////////////////////////////////
	TheInput::Singleton()->ReadInputStates();
	ListenForKeys();

	///////////////////////////////////////////
	// SkyBox
	///////////////////////////////////////////
	g_SkyBox->Draw(&g_GameCamera->GetPosition(), &view, &projection);

	///////////////////////////////////////////
	// Nodes
	///////////////////////////////////////////
	g_root_node->execute(&world, &view, &projection);

	g_EarthNode->Rotate(0.0002f);
	g_O2CannisterNode->Rotate(0.02f); 
	g_O2CannisterNode2->Rotate(0.02f);
	g_O2CannisterNode3->Rotate(0.02f);

	///////////////////////////////////////////
	// Text / HUD
	///////////////////////////////////////////
	g_2DText->AddText("Health " + to_string((int)g_Hud->getHealth()), -1.0, -0.8, 0.05);
	g_2DText->AddText("O2 Levels " + to_string((int)g_Hud->getO2()), -1.0, -0.9, 0.05);
	if (hasQuestItem == true){
		g_2DText->AddText("Quest Item 1", -1.0, 1.0, 0.05);
	}
	g_2DText->RenderText();
	
	
}


void PlayState::ListenForKeys() {

	///////////////////////////////////////////
	// ESC Application
	///////////////////////////////////////////
	if (TheInput::Singleton()->IsKeyPressed(DIK_ESCAPE))				// ESC
		DestroyWindow(g_hWnd);

	///////////////////////////////////////////
	// CameraControls
	///////////////////////////////////////////

	if (TheInput::Singleton()->IsKeyPressed(DIK_W)) {					// FORWARD
		g_GameCamera->Forward(0.040f);
		// set Camera node to the position of the Camera
		g_CameraNode->SetPos(g_GameCamera->getX(), g_GameCamera->getY(), g_GameCamera->getZ());

		g_Hud->setO2(0.001f);

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new Camera position
		g_root_node->Update_Collision_Tree(&identity, 1.0);

		if (g_CameraNode->check_collision(g_root_node))
		{
			if (g_CameraNode->check_collision(g_O2CannisterNode)) {
				g_O2CannisterNode->draw = false;
				g_PickUp->pickUpO2Object();
				
			}

			if (g_CameraNode->check_collision(g_QuestItemNode)) {
				g_QuestItemNode->draw = false;
				g_PickUp->pickUpQuestObject();

			}

			if (g_CameraNode->check_collision(g_RockNode)){

				g_RockNode->IncZ(0.0002f, g_root_node);
	
			}


			g_Hud->setO2(0.1f);

			if (g_Hud->getO2() <= 0) {

				g_Hud->setHealth(0.1f);

				if (g_Hud->getHealth() <= 0)
				{
					TheGame::Singleton()->getStateMachine()->changeState(new GameOverState());
				}

			}

			// if there is a collision, restore Camera and Camera node positions
			g_GameCamera->Forward(-.3);
			g_CameraNode->SetPos(g_GameCamera->getX(), g_GameCamera->getY(), g_GameCamera->getZ());
			//g_root_node->Update_Collision_Tree(&identity, 1.0);
		}

			if (g_Hud->getO2() <= 0) {

				g_Hud->setHealth(0.001f);

				if (g_Hud->getHealth() <= 0)
				{
					TheGame::Singleton()->getStateMachine()->changeState(new GameOverState());
				}

			}

	}

	if (TheInput::Singleton()->IsKeyPressed(DIK_A)) {					 // LEFT
		g_GameCamera->RotateLeft(0.05f);

		if (g_Hud->getHealth() <= 0)
		{
			TheGame::Singleton()->getStateMachine()->changeState(new GameOverState());
		}

		// set Camera node to the position of the Camera
		g_CameraNode->SetPos(g_GameCamera->getX(), g_GameCamera->getY(), g_GameCamera->getZ());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new Camera position
		g_root_node->Update_Collision_Tree(&identity, 1.0);

		if (g_CameraNode->check_collision(g_root_node))
		{
			g_Hud->setO2(0.1f);

			if (g_Hud->getO2() <= 0) {

				g_Hud->setHealth(0.1f);

				if (g_Hud->getHealth() <= 0)
				{
					TheGame::Singleton()->getStateMachine()->changeState(new GameOverState());
				}

			}
			// if there is a collision, restore Camera and Camera node positions
			g_GameCamera->Forward(-.3);
			g_CameraNode->SetPos(g_GameCamera->getX(), g_GameCamera->getY(), g_GameCamera->getZ());
			//g_root_node->Update_Collision_Tree(&identity, 1.0);
		}

	}

	if (TheInput::Singleton()->IsKeyPressed(DIK_S)) {					// BACK
		g_GameCamera->Back(0.040f);

		g_Hud->setO2(0.001f);

		if (g_Hud->getO2() <= 0) {

			g_Hud->setHealth(0.001f);

			if (g_Hud->getHealth() <= 0)
			{
				TheGame::Singleton()->getStateMachine()->changeState(new GameOverState());
			}

		}

		// set Camera node to the position of the Camera
		g_CameraNode->SetPos(g_GameCamera->getX(), g_GameCamera->getY(), g_GameCamera->getZ());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new Camera position
		g_root_node->Update_Collision_Tree(&identity, 1.0);

		if (g_CameraNode->check_collision(g_root_node))
		{
			g_Hud->setO2(0.1f);

			if (g_Hud->getO2() <= 0) {

				g_Hud->setHealth(0.1f);

				if (g_Hud->getHealth() <= 0)
				{
					TheGame::Singleton()->getStateMachine()->changeState(new GameOverState());
				}

			}
			// if there is a collision, restore Camera and Camera node positions
			g_GameCamera->Forward(-.3);
			g_CameraNode->SetPos(g_GameCamera->getX(), g_GameCamera->getY(), g_GameCamera->getZ());
			//g_root_node->Update_Collision_Tree(&identity, 1.0);
		}

	}


	if (TheInput::Singleton()->IsKeyPressed(DIK_D)) {					 // RIGHT
		g_GameCamera->RotateRight(0.05f);

		if (g_Hud->getHealth() <= 0)
		{
			TheGame::Singleton()->getStateMachine()->changeState(new GameOverState());
		}

		// set Camera node to the position of the Camera
		g_CameraNode->SetPos(g_GameCamera->getX(), g_GameCamera->getY(), g_GameCamera->getZ());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new Camera position
		g_root_node->Update_Collision_Tree(&identity, 1.0);

		if (g_CameraNode->check_collision(g_root_node))
		{
			g_Hud->setO2(0.1f);

			if (g_Hud->getO2() <= 0) {

				g_Hud->setHealth(0.1f);

				if (g_Hud->getHealth() <= 0)
				{
					TheGame::Singleton()->getStateMachine()->changeState(new GameOverState());
				}

			}
			// if there is a collision, restore Camera and Camera node positions
			g_GameCamera->Forward(-.3);
			g_CameraNode->SetPos(g_GameCamera->getX(), g_GameCamera->getY(), g_GameCamera->getZ());
			//g_root_node->Update_Collision_Tree(&identity, 1.0);
		}

	}

	if (TheInput::Singleton()->IsKeyPressed(DIK_SPACE)) {					// JUMP
		g_GameCamera->Jump(0.040f);
		// set Camera node to the position of the Camera
		g_CameraNode->SetPos(g_GameCamera->getX(), g_GameCamera->getY(), g_GameCamera->getZ());

		g_Hud->setO2(0.001f);

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new Camera position
		g_root_node->Update_Collision_Tree(&identity, 1.0);

		if (g_CameraNode->check_collision(g_root_node))
		{
			g_Hud->setO2(0.1f);

			if (g_Hud->getO2() <= 0) {

				g_Hud->setHealth(0.1f);

				if (g_Hud->getHealth() <= 0)
				{
					TheGame::Singleton()->getStateMachine()->changeState(new GameOverState());
				}

			}
			// if there is a collision, restore Camera and Camera node positions
			g_GameCamera->Forward(-.3);
			g_CameraNode->SetPos(g_GameCamera->getX(), g_GameCamera->getY(), g_GameCamera->getZ());
			//g_root_node->Update_Collision_Tree(&identity, 1.0);
		}

		if (g_Hud->getO2() <= 0) {

			g_Hud->setHealth(0.001f);

			if (g_Hud->getHealth() <= 0)
			{
				TheGame::Singleton()->getStateMachine()->changeState(new GameOverState());
			}

		}

	}

	if (TheInput::Singleton()->IsKeyPressed(DIK_C)) {					 // DOWN
		g_GameCamera->Down(0.041f);

		if (g_Hud->getHealth() <= 0)
		{
			TheGame::Singleton()->getStateMachine()->changeState(new GameOverState());
		}

		// set Camera node to the position of the Camera
		g_CameraNode->SetPos(g_GameCamera->getX(), g_GameCamera->getY(), g_GameCamera->getZ());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new Camera position
		g_root_node->Update_Collision_Tree(&identity, 1.0);

		if (g_CameraNode->check_collision(g_root_node))
		{
			g_Hud->setO2(0.1f);

			if (g_Hud->getO2() <= 0) {

				g_Hud->setHealth(0.1f);

				if (g_Hud->getHealth() <= 0)
				{
					TheGame::Singleton()->getStateMachine()->changeState(new GameOverState());
				}

			}
			// if there is a collision, restore Camera and Camera node positions
			g_GameCamera->Forward(-.3);
			g_CameraNode->SetPos(g_GameCamera->getX(), g_GameCamera->getY(), g_GameCamera->getZ());
			//g_root_node->Update_Collision_Tree(&identity, 1.0);
		}

	}

	if (TheInput::Singleton()->IsKeyPressed(DIK_LSHIFT)) {					 // RUN
		g_GameCamera->Run(0.062f);

		g_Hud->setO2(0.001f);

		if (g_Hud->getO2() <= 0) {

			g_Hud->setHealth(0.001f);

			if (g_Hud->getHealth() <= 0)
			{
				TheGame::Singleton()->getStateMachine()->changeState(new GameOverState());
			}

			// set Camera node to the position of the Camera
			g_CameraNode->SetPos(g_GameCamera->getX(), g_GameCamera->getY(), g_GameCamera->getZ());

			XMMATRIX identity = XMMatrixIdentity();

			// update tree to reflect new Camera position
			g_root_node->Update_Collision_Tree(&identity, 1.0);

			if (g_CameraNode->check_collision(g_root_node))
			{
				/*if (g_CameraNode->check_collision(g_O2CannisterNode)) {

					g_PickUp->pickUpO2Object();

				}

				if (g_CameraNode->check_collision(g_O2CannisterNode)) {

					g_PickUp->pickUpQuestObject();

				}*/


				g_Hud->setO2(0.1f);

				if (g_Hud->getO2() <= 0) {

					g_Hud->setHealth(0.1f);

					if (g_Hud->getHealth() <= 0)
					{
						TheGame::Singleton()->getStateMachine()->changeState(new GameOverState());
					}

				}
				// if there is a collision, restore Camera and Camera node positions
				g_GameCamera->Forward(-.3);
				g_CameraNode->SetPos(g_GameCamera->getX(), g_GameCamera->getY(), g_GameCamera->getZ());
				//g_root_node->Update_Collision_Tree(&identity, 1.0);
			}

		}
	}

}


bool PlayState::onEnter(){

	//////////////////////////////////////////////////////////////////////////////////////
	//						Init Graphics
	//////////////////////////////////////////////////////////////////////////////////////

		HRESULT hr = S_OK;

		//////////////////////////////////////////////////////////////////////////////////////
		//									Init Objects
		//////////////////////////////////////////////////////////////////////////////////////
		hasQuestItem = false;

		g_Hud = new HUD(100, 100);
		g_PickUp = new PickUp(g_ModelO2Cannister, g_Hud, this);

		///////////////////////////////////////////
		// Models
		///////////////////////////////////////////
		g_ModelCamera = new Model();
		g_ModelFloor = new Model();
		g_ModelRock = new Model();
		g_ModelRock2 = new Model();
		g_ModelRock3 = new Model();
		g_ModelRock4 = new Model();
		g_ModelEarth = new Model(); 
		g_ModelMoon = new Model();
		g_ModelSun = new Model(); 
		g_ModelO2Cannister = new Model(); 
		g_ModelO2Cannister2 = new Model();
		g_ModelO2Cannister3 = new Model();
		g_ModelQuestItem = new Model();
		g_ModelQuestItem2 = new Model();
		g_ModelQuestItem3 = new Model();

		///////////////////////////////////////////
		// Nodes
		///////////////////////////////////////////
		g_root_node = new SceneNode();
		g_CameraNode = new SceneNode(g_ModelCamera);
		g_LevelNode = new SceneNode(g_ModelFloor);
		g_RockNode = new SceneNode(g_ModelRock);
		g_RockNode2 = new SceneNode(g_ModelRock2);
		g_RockNode3 = new SceneNode(g_ModelRock3);
		g_RockNode4 = new SceneNode(g_ModelRock4);
		g_EarthNode = new SceneNode(g_ModelEarth);
		g_MoonNode = new SceneNode(g_ModelMoon);
		g_SunNode = new SceneNode(g_ModelSun); 
		g_O2CannisterNode = new PickUp(g_ModelO2Cannister, g_Hud, this);
		g_O2CannisterNode2 = new PickUp(g_ModelO2Cannister2, g_Hud, this);
		g_O2CannisterNode3 = new PickUp(g_ModelO2Cannister3, g_Hud, this);
		g_QuestItemNode = new PickUp(g_ModelQuestItem, g_Hud, this);
		g_QuestItemNode2 = new PickUp(g_ModelQuestItem2, g_Hud, this);
		g_QuestItemNode3 = new PickUp(g_ModelQuestItem3, g_Hud, this);

		///////////////////////////////////////////
		// OTHER
		///////////////////////////////////////////
		g_GameCamera = new Camera(0.0f, 0.0f, 0.0f, 0.0f);
		g_SkyBox = new SkyBox();
		g_2DText = new Text2D("Assets/Fonts/font1.bmp", TheGame::Singleton()->getDevice(), TheGame::Singleton()->getContext());

		//////////////////////////////////////////////////////////////////////////////////////
		//								Node / Object Relationships
		//////////////////////////////////////////////////////////////////////////////////////
		g_root_node->addChildNode(g_CameraNode);
		g_root_node->addChildNode(g_LevelNode);
		g_LevelNode->addChildNode(g_RockNode);
		g_LevelNode->addChildNode(g_RockNode2);
		g_LevelNode->addChildNode(g_RockNode3);
		g_LevelNode->addChildNode(g_RockNode4);
		g_LevelNode->addChildNode(g_EarthNode);
		g_LevelNode->addChildNode(g_O2CannisterNode);
		g_LevelNode->addChildNode(g_O2CannisterNode2);
		g_LevelNode->addChildNode(g_O2CannisterNode3);
		g_LevelNode->addChildNode(g_QuestItemNode);
		g_LevelNode->addChildNode(g_QuestItemNode2);
		g_LevelNode->addChildNode(g_QuestItemNode3);
		g_EarthNode->addChildNode(g_MoonNode);
		g_EarthNode->addChildNode(g_SunNode);
		

		//////////////////////////////////////////////////////////////////////////////////////
		//									Load Objects
		//////////////////////////////////////////////////////////////////////////////////////
		g_SkyBox->LoadObjModel("Assets/Objects/cube.obj", "sky_shader.hlsl");
		g_ModelCamera->LoadObjModel("Assets/Objects/cube.obj", "model_shaders.hlsl");
		g_ModelFloor->LoadObjModel("Assets/Objects/floor.obj", "model_shaders.hlsl");
		g_ModelRock->LoadObjModel("Assets/Objects/cube.obj", "model_shaders.hlsl");
		g_ModelRock2->LoadObjModel("Assets/Objects/cube.obj", "model_shaders.hlsl");
		g_ModelRock3->LoadObjModel("Assets/Objects/cube.obj", "model_shaders.hlsl");
		g_ModelRock4->LoadObjModel("Assets/Objects/cube.obj", "model_shaders.hlsl");
		g_ModelEarth->LoadObjModel("Assets/Objects/Sphere.obj", "model_shaders.hlsl");
		g_ModelMoon->LoadObjModel("Assets/Objects/Moon.obj", "model_shaders.hlsl");
		g_ModelSun->LoadObjModel("Assets/Objects/Sun.obj", "model_shaders.hlsl");
		g_ModelO2Cannister->LoadObjModel("Assets/Objects/cube.obj", "model_shaders.hlsl"); 
		g_ModelO2Cannister2->LoadObjModel("Assets/Objects/cube.obj", "model_shaders.hlsl");
		g_ModelO2Cannister3->LoadObjModel("Assets/Objects/cube.obj", "model_shaders.hlsl");
		g_ModelQuestItem->LoadObjModel("Assets/Objects/cube.obj", "model_shaders.hlsl");
		g_ModelQuestItem2->LoadObjModel("Assets/Objects/cube.obj", "model_shaders.hlsl");
		g_ModelQuestItem3->LoadObjModel("Assets/Objects/cube.obj", "model_shaders.hlsl");


		//////////////////////////////////////////////////////////////////////////////////////
		//									Add Textures
		//////////////////////////////////////////////////////////////////////////////////////
		g_SkyBox->AddTexture("Assets/SkyBox/Mars.dds");
		g_ModelFloor->AddTexture("Assets/Textures/Rock.bmp");
		g_ModelRock->AddTexture("Assets/Textures/Rock.bmp");
		g_ModelRock2->AddTexture("Assets/Textures/Rock.bmp");
		g_ModelRock3->AddTexture("Assets/Textures/Rock.bmp");
		g_ModelRock4->AddTexture("Assets/Textures/Rock.bmp");
		g_ModelEarth->AddTexture("Assets/Textures/earth.bmp");
		g_ModelMoon->AddTexture("Assets/Textures/moon.bmp");
		g_ModelSun->AddTexture("Assets/Textures/sun.bmp");
		g_ModelO2Cannister->AddTexture("Assets/Textures/o2.bmp"); 
		g_ModelO2Cannister2->AddTexture("Assets/Textures/o2.bmp");
		g_ModelO2Cannister3->AddTexture("Assets/Textures/o2.bmp");
		g_ModelQuestItem->AddTexture("Assets/Textures/questItem.bmp");
		g_ModelQuestItem2->AddTexture("Assets/Textures/questItem.bmp");
		g_ModelQuestItem3->AddTexture("Assets/Textures/questItem.bmp");

		//////////////////////////////////////////////////////////////////////////////////////
		//									World Translations
		//////////////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////
		// Nodes
		///////////////////////////////////////////
		g_root_node->SetPos(0, 0, 0);
		g_LevelNode->SetPos(0.0f, -10.0f, 40.0f);
		g_RockNode->SetPos(-45.0f, 0.0f, 70.0f);
		g_RockNode2->SetPos(35.0f, 0.0f, 35.0f);
		g_RockNode3->SetPos(20.0f, 0.0f, 65.0f);
		g_RockNode4->SetPos(-50.0f, 0.0f, 15.0f);
		g_O2CannisterNode->SetPos(0.0f, 10.0f, 25.0f); 
		g_O2CannisterNode2->SetPos(-20.0f, 10.0f, 45.0f);
		g_O2CannisterNode3->SetPos(40.0f, 10.0f, 65.0f);
		g_QuestItemNode->SetPos(60.0f, 20.0f, 105.0f);
		g_QuestItemNode2->SetPos(-60.0f, 20.0f, 105.0f);
		g_QuestItemNode3->SetPos(20.0f, 20.0f, 105.0f);

		///////////////////////////////////////////
		// Solar System
		///////////////////////////////////////////
		g_EarthNode->SetPos(100.0f, 200.0f, 500.0f);
		g_MoonNode->SetPos(120.0f, 190.0f, 500.0f);
		g_SunNode->SetPos(105.0f, 200.0f, 500.0f);

		///////////////////////////////////////////
		// SkyBox
		///////////////////////////////////////////
		g_SkyBox->SetPos(g_GameCamera->getX(), g_GameCamera->getY(), g_GameCamera->getZ());
		g_SkyBox->SetScale(10000.0f);

		///////////////////////////////////////////
		// Camera
		///////////////////////////////////////////
		g_CameraNode->SetPos(g_GameCamera->getX(), g_GameCamera->getY(), g_GameCamera->getZ()); // Need to minus on the Z cause it spawns in front of you
		//g_CameraNode->SetScale(1.0f);

		///////////////////////////////////////////
		// Rocks
		///////////////////////////////////////////
		g_MoonNode->SetScale(2.0f);


	return true; 
}

bool PlayState::onExit(){
	

	std::cout << "Exiting PlayState\n";
	return true;
}



