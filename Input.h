#pragma once
#include <dinput.h>
#include <xinput.h>

class Input
{
public:
	static Input* Singleton(){
		if (s_pInstance == 0){
			s_pInstance = new Input();
			return s_pInstance;
		}
		return s_pInstance;
	}

		Input();
		~Input();

		HRESULT InitialiseInput(HINSTANCE *g_hInst, HWND *g_hWnd);

		bool IsKeyPressed(unsigned char DI_Keycode);
		void ListenForKeys();
		void ReadInputStates();

		bool Init(HINSTANCE* g_wInst, HWND* g_hWnd);


private:
	static Input* s_pInstance;

	IDirectInput8*			g_direct_input;
	IDirectInputDevice8*	g_keyboard_device;
	unsigned char			g_keyboard_keys_state[256];


};
typedef Input TheInput;
