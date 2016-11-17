#include "../Header/Game.h"
#include "../Header/func.h"
#include "../Header/Direct3D11.h"
#include <memory>
#include "../Header/GameObject.h"
//
//�O���[�o���ϐ��錾
HINSTANCE g_hInst;
unique_ptr<Game> g_pGame = nullptr;

TCHAR* g_WindowName = _T("Billiards");

// �v���g�^�C�v�錾
void CreateDirect3D(HINSTANCE hInstance);

//�G���g���[�|�C���g
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//��x���g��Ȃ����ߌx��������邽��
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_EVERY_1024_DF);		//���������[�N���N���������̃`�F�b�N
#endif

	// Direct3D�̍쐬
	CreateDirect3D(hInstance);

	ShowWindow(Direct3D11::Instance().hWnd, SW_SHOW);
	UpdateWindow(Direct3D11::Instance().hWnd);

	// �V�X�e���̏�����
	g_pGame = make_unique<Game>();
	
	// ���b�Z�[�W���[�v
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

// �E�B���h�E�v���V�[�W��
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	switch (msg)
	{
	case WM_KEYUP:

		// �A�v���I��
		if (wParam == VK_ESCAPE)
			
			// �w�肳�ꂽ�E�B���h�E��j��
			DestroyWindow(hWnd);

		break;

	case WM_DESTROY:
		
		// �w�肳�ꂽ�X���b�h������̏I����v���������Ƃ��V�X�e���ɓ`����
		PostQuitMessage(0);

		break;

	default:

		// ����̃E�B���h�E�v���V�[�W�����Ăяo���āA�A�v���P�[�V�������������Ȃ��E�B���h�E���b�Z�[�W�ɑ΂��āA����̏�����񋟂��܂�
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

void CreateDirect3D(HINSTANCE hInstance)
{
	unique_ptr<DXGI_MODE_DESC> pModeDescArray = nullptr;

	DXGI_MODE_DESC  ModeDesc;
	UINT num;

	// �f�B�X�v���C���[�h�ꗗ�̐����擾����
	Direct3D11::Instance().GetDisplayMode(nullptr, &num);

	pModeDescArray.reset(NEW DXGI_MODE_DESC[num]);
	// �f�B�X�v���C���[�h�ꗗ���擾����
	Direct3D11::Instance().GetDisplayMode(pModeDescArray.get(), &num);

	bool find = false;
	for (UINT i = 0; i<num; i++)
	{
		// �K�؂ȉ𑜓x�̃f�B�X�v���C���[�h����������
		if (pModeDescArray.get()[i].Width == WINDOW_WIDTH && pModeDescArray.get()[i].Height == WINDOW_HEIGHT)
		{
			find = true;
			CopyMemory(&ModeDesc, &pModeDescArray.get()[i], sizeof(DXGI_MODE_DESC));
			break;
		}
	}

	if (find == false)
		throw(UException(-1, _T("InitDirect3D()�ŃG���[���������܂����B�K�؂ȃf�B�X�v���C���[�h�̉𑜓x���擾�ł��܂���B")));

	// �E�B���h�E�̍쐬�����Direct3D �̍쐬
	Direct3D11::Instance().CreateDirect3D11(g_WindowName, hInstance, WndProc, &ModeDesc, TRUE, TRUE);
}