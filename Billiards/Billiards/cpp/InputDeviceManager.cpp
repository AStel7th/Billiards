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

	// マウス用にデバイスオブジェクトを作成
	hr = pDInput->CreateDevice(GUID_SysMouse, &pDIMouse, nullptr);
	if (FAILED(hr)) {
		// デバイスの作成に失敗
		return false;
	}

	// データフォーマットを設定
	hr = pDIMouse->SetDataFormat(&c_dfDIMouse);	// マウス用のデータ・フォーマットを設定
	if (FAILED(hr)) {
		// データフォーマットに失敗
		return false;
	}

	// モードを設定（フォアグラウンド＆非排他モード）
	hr = pDIMouse->SetCooperativeLevel(Direct3D11::Instance().GetHWND(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr)) {
		// モードの設定に失敗
		return false;
	}

	// デバイスの設定
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL;	// 相対値モードで設定（絶対値はDIPROPAXISMODE_ABS）

	hr = pDIMouse->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	if (FAILED(hr)) {
		// デバイスの設定に失敗
		return false;
	}

	// 入力制御開始
	pDIMouse->Acquire();


	//キーボード用にデバイスオブジェクトを作成
	hr = pDInput->CreateDevice(GUID_SysKeyboard, &pDIKeyboard, nullptr);
	if (FAILED(hr))
		return false;  // デバイスの作成に失敗

					   //キーボード用のデータ・フォーマットを設定
	hr = pDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
		return false; // デバイスの作成に失敗

					  //モードを設定（フォアグラウンド＆非排他モード）
	hr = pDIKeyboard->SetCooperativeLevel(Direct3D11::Instance().GetHWND(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr))
		return false; // モードの設定に失敗

					  //キーボード入力制御開始
	pDIKeyboard->Acquire();

	return S_OK;
}

void InputDeviceManager::Update()
{
	// 読取前の値を保持します
	DIMOUSESTATE zdiMouseState_bak;	// マウス情報(変化検知用)
	memcpy(&zdiMouseState_bak, &ms, sizeof(zdiMouseState_bak));

	// ここから、DirectInputで必要なコード -->
	// マウスの状態を取得します
	HRESULT	hr = pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &ms);
	if (hr == DIERR_INPUTLOST) {
		pDIMouse->Acquire();
		hr = pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &ms);
	}
	// --> ここまで、DirectInputで必要なコード

	//if (memcmp(&zdiMouseState_bak, &ms, sizeof(zdiMouseState_bak)) != 0) {
	//	// 確認用の処理、ここから -->
	//	// 値が変わったら表示します
	//	TCHAR buf[128];
	//	_stprintf_s(buf, _T("(%5d, %5d, %5d) %s %s %s\n"),
	//		ms.lX, ms.lY, ms.lZ,
	//		(ms.rgbButtons[0] & 0x80) ? "Left" : "--",
	//		(ms.rgbButtons[1] & 0x80) ? "Right" : "--",
	//		(ms.rgbButtons[2] & 0x80) ? "Center" : "--");
	//	OutputDebugString(buf);
	//	// --> ここまで、確認用の処理
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

		// クライアント領域を画面座標に変換する
		POINT pt = { rc.left, rc.top };
		POINT pt2 = { rc.right, rc.bottom };
		ClientToScreen(Direct3D11::Instance().GetHWND(), &pt);
		ClientToScreen(Direct3D11::Instance().GetHWND(), &pt2);
		SetRect(&rc, pt.x, pt.y, pt2.x, pt2.y);

		// カーソルの動作範囲を制限する
		ClipCursor(&rc);
	}
	else
	{
		ClipCursor(nullptr);
	}
}
