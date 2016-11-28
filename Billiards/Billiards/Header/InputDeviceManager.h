#pragma once
#include <windows.h>
#include <dinput.h>
#include <tchar.h>
#include "func.h"

#define DIRECTINPUT_VERSION		0x0800		// DirectInput�̃o�[�W�������

class InputDeviceManager
{
private:
	LPDIRECTINPUT8			pDInput;		// DirectInput�I�u�W�F�N�g
	LPDIRECTINPUTDEVICE8	pDIKeyboard;	// �L�[�{�[�h�f�o�C�X
	LPDIRECTINPUTDEVICE8	pDIMouse;		// �}�E�X�f�o�C�X
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

	// �����̓{�^���̎��
	// 0 = Left
	// 1 = Right
	// 3 = Center
	bool GetMouseButtonDown(int num);
};