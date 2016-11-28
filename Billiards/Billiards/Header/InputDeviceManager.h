#pragma once
#include <windows.h>
#include <dinput.h>
#include <tchar.h>
#include "func.h"

#define DIRECTINPUT_VERSION		0x0800		// DirectInputのバージョン情報

class InputDeviceManager
{
private:
	LPDIRECTINPUT8			pDInput;		// DirectInputオブジェクト
	LPDIRECTINPUTDEVICE8	pDIKeyboard;	// キーボードデバイス
	LPDIRECTINPUTDEVICE8	pDIMouse;		// マウスデバイス
	DIMOUSESTATE			ms;				// mouse state 
protected:
	InputDeviceManager();
public:
	static InputDeviceManager& Instance()
	{
		static InputDeviceManager inst;
		return inst;
	}

	virtual ~InputDeviceManager();

	HRESULT Init(HINSTANCE hinstance);

	void Update();

	XMFLOAT2 GetMouseState();

	// 引数はボタンの種類
	// 0 = Left
	// 1 = Right
	// 3 = Center
	bool GetMouseButtonDown(int num);
};