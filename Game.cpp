#include "Game.h"
#include "MenuState.h"
#include "PlayState.h"

Game* Game::s_pInstance = 0;

const int FPS = 60;
const int DELAY_TIME = 1000.0f / FPS;

Game::Game()
{

}


Game::~Game()
{
}

bool Game::Init(ID3D11Device* Device, ID3D11DeviceContext* Context) {

	g_pD3DDevice = Device;
	g_pImmediateContext = Context;

	m_pGameStateMachine = new GameStateMachine();
	m_pGameStateMachine->changeState(new MenuState());

	return true;
}

void Game::render(){


	m_pGameStateMachine->render();


}


void Game::update(){

	m_pGameStateMachine->update();

}

void Game::handleEvents(){

	/*TheInputHandler::Singleton()->update();

	if (TheInputHandler::Singleton()->isKeyDown(SDL_SCANCODE_RETURN)){
		m_pGameStateMachine->changeState(new PlayState());
	}*/


}

void Game::clean(){



}

//void Game::FPSCounter(){
//	frameStart = SDL_GetTicks();
//	frameTime = SDL_GetTicks() - frameStart;
//
//	if (frameTime < DELAY_TIME){
//		SDL_Delay((int)(DELAY_TIME - frameTime));
//		std::cout << DELAY_TIME - frameTime << "\n";
//	}
//
//}