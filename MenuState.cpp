#include "MenuState.h"
#include "GameStateMachine.h"
#include "Game.h"
#include "PlayState.h"

const std::string MenuState::s_menuID = "MENU";

void MenuState::update(){
	
	bitmap->RenderText();

	TheInput::Singleton()->ReadInputStates();

	if (TheInput::Singleton()->IsKeyPressed(DIK_SPACE)){
		
		TheGame::Singleton()->getStateMachine()->changeState(new PlayState());

	}
}

void MenuState::render(){
	

}

bool MenuState::onEnter(){

	bitmap = new Bitmap("Assets/MarsScreen.bmp", -1, 1, 2, 2);

	return true;
}

bool MenuState::onExit(){
	
	
	return true;

}

void MenuState::s_menuToPlay(){
	
	TheGame::Singleton()->getStateMachine()->changeState(new PlayState());
}

void MenuState::s_exitFromMenu(){
	
	TheGame::Singleton()->clean(); 
}

