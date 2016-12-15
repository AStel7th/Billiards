#include "../Header/InputDeviceManager.h"
#include "../Header/Direct3D11.h"

InputDeviceManager::InputDeviceManager()
{
}

InputDeviceManager::~InputDeviceManager()
{
	if (pDIMouse)
		pDIMouse->Unacquire();

	if (pDIKeyboard)
		pDIKeyboard->Unacquire();

	SAFE_RELEASE(pDInput);
	SAFE_RELEASE(pDIMouse);
	SAFE_RELEASE(pDIKeyboard);
}

HRESULT InputDeviceManager::Init(HINSTANCE hinstance)
{
	HRESULT hr;

	if (FAILED(hr = DirectInput8Create(hinstance, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (VOID**)&pDInput, nullptr)))
		return hr;

	// �}�E�X�p�Ƀf�o�C�X�I�u�W�F�N�g���쐬
	hr = pDInput->CreateDevice(GUID_SysMouse, &pDIMouse, nullptr);
	if (FAILED(hr)) {
		// �f�o�C�X�̍쐬�Ɏ��s
		return false;
	}

	// �f�[�^�t�H�[�}�b�g��ݒ�
	hr = pDIMouse->SetDataFormat(&c_dfDIMouse);	// �}�E�X�p�̃f�[�^�E�t�H�[�}�b�g��ݒ�
	if (FAILED(hr)) {
		// �f�[�^�t�H�[�}�b�g�Ɏ��s
		return false;
	}

	// ���[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	hr = pDIMouse->SetCooperativeLevel(Direct3D11::Instance().GetHWND(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr)) {
		// ���[�h�̐ݒ�Ɏ��s
		return false;
	}

	// �f�o�C�X�̐ݒ�
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL;	// ���Βl���[�h�Őݒ�i��Βl��DIPROPAXISMODE_ABS�j

	hr = pDIMouse->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	if (FAILED(hr)) {
		// �f�o�C�X�̐ݒ�Ɏ��s
		return false;
	}

	// ���͐���J�n
	pDIMouse->Acquire();


	//�L�[�{�[�h�p�Ƀf�o�C�X�I�u�W�F�N�g���쐬
	hr = pDInput->CreateDevice(GUID_SysKeyboard, &pDIKeyboard, nullptr);
	if (FAILED(hr))
		return false;  // �f�o�C�X�̍쐬�Ɏ��s

					   //�L�[�{�[�h�p�̃f�[�^�E�t�H�[�}�b�g��ݒ�
	hr = pDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
		return false; // �f�o�C�X�̍쐬�Ɏ��s

					  //���[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	hr = pDIKeyboard->SetCooperativeLevel(Direct3D11::Instance().GetHWND(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr))
		return false; // ���[�h�̐ݒ�Ɏ��s

					  //�L�[�{�[�h���͐���J�n
	pDIKeyboard->Acquire();

	return S_OK;
}

void InputDeviceManager::Update()
{
	// �ǎ�O�̒l��ێ����܂�
	DIMOUSESTATE zdiMouseState_bak;	// �}�E�X���(�ω����m�p)
	memcpy(&zdiMouseState_bak, &ms, sizeof(zdiMouseState_bak));

	// ��������ADirectInput�ŕK�v�ȃR�[�h -->
	// �}�E�X�̏�Ԃ��擾���܂�
	HRESULT	hr = pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &ms);
	if (hr == DIERR_INPUTLOST) {
		pDIMouse->Acquire();
		hr = pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &ms);
	}
	// --> �����܂ŁADirectInput�ŕK�v�ȃR�[�h

	//if (memcmp(&zdiMouseState_bak, &ms, sizeof(zdiMouseState_bak)) != 0) {
	//	// �m�F�p�̏����A�������� -->
	//	// �l���ς������\�����܂�
	//	TCHAR buf[128];
	//	_stprintf_s(buf, _T("(%5d, %5d, %5d) %s %s %s\n"),
	//		ms.lX, ms.lY, ms.lZ,
	//		(ms.rgbButtons[0] & 0x80) ? "Left" : "--",
	//		(ms.rgbButtons[1] & 0x80) ? "Right" : "--",
	//		(ms.rgbButtons[2] & 0x80) ? "Center" : "--");
	//	OutputDebugString(buf);
	//	// --> �����܂ŁA�m�F�p�̏���
	//}
}

XMFLOAT2 InputDeviceManager::GetMouseState()
{
	return XMFLOAT2((float)ms.lX,(float)ms.lY);
}

bool InputDeviceManager::GetMouseButtonDown(int num)
{
	if (num < 0 || num > 3)
		return false;

	if (ms.rgbButtons[num] & 0x80)
		return true;
	else
		return false;
}

void InputDeviceManager::ClipCursorWindow(bool b)
{
	if (b)
	{
		RECT rc;
		GetClientRect(Direct3D11::Instance().GetHWND(), &rc);

		// �N���C�A���g�̈����ʍ��W�ɕϊ�����
		POINT pt = { rc.left, rc.top };
		POINT pt2 = { rc.right, rc.bottom };
		ClientToScreen(Direct3D11::Instance().GetHWND(), &pt);
		ClientToScreen(Direct3D11::Instance().GetHWND(), &pt2);
		SetRect(&rc, pt.x, pt.y, pt2.x, pt2.y);

		// �J�[�\���̓���͈͂𐧌�����
		ClipCursor(&rc);
	}
	else
	{
		ClipCursor(nullptr);
	}
}
