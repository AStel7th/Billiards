#include "../Header/Game.h"
#include "../Header/TimeControl.h"
#include "../Header/AudioSystem.h"
#include "../Header/Sprite.h"
#include "../Header/SpaceDivision.h"
#include "../Header/DrawSystem.h"
#include "../Header/Camera.h"
#include "../Header/GameObject.h"
#include "../Header/Collider.h"
#include "../Header/NineBall.h"
//#include "../Header/Effect.h"

Game::Game()
{
	// マウスカーソルをゲーム中は非表示にする
	// ウィンドウのクライアント領域を取得する
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

	AudioSystem::Instance().Create();

	Sprite::Create();
	DrawSystem::Instance().Init(1);

	timeCtrl = NEW TimeControl();

	SpaceDivision::Instance().Init(5,XMFLOAT3(-250.0f, -250.0f, -250.0f), XMFLOAT3(250.0f, 250.0f, 250.0f));

	Camera::Instance().SetView(XMFLOAT3(0.0f, 300.0f, -150.0f), XMFLOAT3(0.0f, 40.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	//Camera::Instance().SetView(XMFLOAT3(1.0f, 200.0f, 0.0f), XMFLOAT3(0.0f, 80.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	Camera::Instance().SetProj(RADIAN(60.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 1.0f, 2000.0f);


	/*EffectManager::Instance().SetView(&Camera::Instance().m_eye, &Camera::Instance().m_at, &Camera::Instance().m_up );
	EffectManager::Instance().SetProj(Camera::Instance().m_fov, Camera::Instance().m_aspect, Camera::Instance().m_zn, Camera::Instance().m_zf);*/

	Create<NineBall>();
}

Game::~Game()
{
	SAFE_DELETE(timeCtrl);
}

bool Game::Run()
{
	timeCtrl->TimeRegular();	//フレーム制御
	timeCtrl->SetFPS(60);

	//EffectManager::Instance().Update();
	GameObject::All::Update();
	Collider::All::HitCheck();


	if (timeCtrl->GetDrawFlag())
	{
		DrawSystem::Instance().Draw();
	}
	
	return true;
}