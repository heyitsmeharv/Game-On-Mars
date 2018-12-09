#pragma once
#include "GameStateMachine.h"
#include <stdint.h> 
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>

using namespace std; 

class Game
{
public:
	static Game* Singleton(){
		if (s_pInstance == 0){
			s_pInstance = new Game();
			return s_pInstance;
		}
		return s_pInstance;
	}
	Game();
	~Game();

	bool Init(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext);

	void update(); 
	void render(); 
	void clean();
	void handleEvents(); 

	ID3D11Device* getDevice() { return g_pD3DDevice; }
	ID3D11DeviceContext* getContext() { return g_pImmediateContext; }

	GameStateMachine* getStateMachine() { return m_pGameStateMachine; }


private:
	static Game* s_pInstance;

	GameStateMachine* m_pGameStateMachine;

	ID3D11Device*				g_pD3DDevice;
	ID3D11DeviceContext*		g_pImmediateContext;


	//Uint32 frameStart, frameTime;

	//void FPSCounter();

};
typedef Game TheGame;