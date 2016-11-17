#include "../Header/Game.h"
#include "../Header/func.h"
#include "../Header/Direct3D11.h"
#include <memory>
#include "../Header/GameObject.h"
//
//グローバル変数宣言
HINSTANCE g_hInst;
unique_ptr<Game> g_pGame = nullptr;

TCHAR* g_WindowName = _T("Billiards");

// プロトタイプ宣言
void CreateDirect3D(HINSTANCE hInstance);

//エントリーポイント
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//一度も使わないため警告を避けるため
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_EVERY_1024_DF);		//メモリリークが起こった時のチェック
#endif

	// Direct3Dの作成
	CreateDirect3D(hInstance);

	ShowWindow(Direct3D11::Instance().hWnd, SW_SHOW);
	UpdateWindow(Direct3D11::Instance().hWnd);

	// システムの初期化
	g_pGame = make_unique<Game>();
	
	// メッセージループ
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_pGame->Run();
		}
	}

	GameObject::All::Clear();

	return (int)msg.wParam;
}

// ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	switch (msg)
	{
	case WM_KEYUP:

		// アプリ終了
		if (wParam == VK_ESCAPE)
			
			// 指定されたウィンドウを破棄
			DestroyWindow(hWnd);

		break;

	case WM_DESTROY:
		
		// 指定されたスレッドが自らの終了を要求したことをシステムに伝える
		PostQuitMessage(0);

		break;

	default:

		// 既定のウィンドウプロシージャを呼び出して、アプリケーションが処理しないウィンドウメッセージに対して、既定の処理を提供します
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

void CreateDirect3D(HINSTANCE hInstance)
{
	unique_ptr<DXGI_MODE_DESC> pModeDescArray = nullptr;

	DXGI_MODE_DESC  ModeDesc;
	UINT num;

	// ディスプレイモード一覧の数を取得する
	Direct3D11::Instance().GetDisplayMode(nullptr, &num);

	pModeDescArray.reset(NEW DXGI_MODE_DESC[num]);
	// ディスプレイモード一覧を取得する
	Direct3D11::Instance().GetDisplayMode(pModeDescArray.get(), &num);

	bool find = false;
	for (UINT i = 0; i<num; i++)
	{
		// 適切な解像度のディスプレイモードを検索する
		if (pModeDescArray.get()[i].Width == WINDOW_WIDTH && pModeDescArray.get()[i].Height == WINDOW_HEIGHT)
		{
			find = true;
			CopyMemory(&ModeDesc, &pModeDescArray.get()[i], sizeof(DXGI_MODE_DESC));
			break;
		}
	}

	if (find == false)
		throw(UException(-1, _T("InitDirect3D()でエラーが発生しました。適切なディスプレイモードの解像度を取得できません。")));

	// ウィンドウの作成およびDirect3D の作成
	Direct3D11::Instance().CreateDirect3D11(g_WindowName, hInstance, WndProc, &ModeDesc, TRUE, TRUE);
}