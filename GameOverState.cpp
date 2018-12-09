#include "GameOverState.h"
#include "MenuState.h"
#include "PlayState.h"
#include "Game.h"
#include "Input.h"


const std::string GameOverState::s_gameOverID = "GAMEOVER";

void GameOverState::update(){

	bitmap->RenderText();

	TheInput::Singleton()->ReadInputStates();

	if (TheInput::Singleton()->IsKeyPressed(DIK_R)){

		s_gameOverToMain();

	}

}

void GameOverState::render(){
	
}

void GameOverState::s_gameOverToMain(){
	TheGame::Singleton()->getStateMachine()->changeState(new MenuState());
}

void GameOverState::s_restartPlay(){
	TheGame::Singleton()->getStateMachine()->changeState(new PlayState());
}

bool GameOverState::onEnter(){
	
	bitmap = new Bitmap("Assets/EndScreen.bmp", -1, 1, 2, 2);

	return true;

}

bool GameOverState::onExit(){

	//std::cout << "exiting GameOverState\n";

	return true;

}
