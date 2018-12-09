#include "Input.h"

Input* Input::s_pInstance = 0;

Input::Input() { 


}

Input::~Input()
{
	if (g_keyboard_device) {

		g_keyboard_device->Unacquire();
		g_keyboard_device->Release();
	}
}


HRESULT Input::InitialiseInput(HINSTANCE *g_hInst, HWND *g_hWnd) {

	HRESULT hr;
	ZeroMemory(g_keyboard_keys_state, sizeof(g_keyboard_keys_state));

	hr = DirectInput8Create(*g_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_direct_input, NULL);
	if (FAILED(hr)) return hr;

	hr = g_direct_input->CreateDevice(GUID_SysKeyboard, &g_keyboard_device, NULL);
	if (FAILED(hr)) return hr;

	hr = g_keyboard_device->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) return hr;

	hr = g_keyboard_device->SetCooperativeLevel(*g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) return hr;

	hr = g_keyboard_device->Acquire();
	if (FAILED(hr)) return hr;


	return S_OK;

}

void Input::ReadInputStates() {

	HRESULT hr;
	hr = g_keyboard_device->GetDeviceState(sizeof(g_keyboard_keys_state), (LPVOID)&g_keyboard_keys_state);

	if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED)) {

		g_keyboard_device->Acquire();
	}


}

bool Input::IsKeyPressed(unsigned char DI_Keycode) {

	return g_keyboard_keys_state[DI_Keycode] & 0x80;
}

