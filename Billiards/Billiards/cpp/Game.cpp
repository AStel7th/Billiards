#include "../Header/Game.h"
#include "../Header/FBXRendererDX11.h"
#include "../Header/Direct3D11.h"
#include "../Header/DrawSystem.h"
#include <memory>
#include "../Header/Camera.h"
//#include "../Header/Effect.h"


Game::Game()
{
	//Sprite::Create();
	DrawSystem::Instance().Init(1);

	timeCtrl = make_unique<TimeControl>();

	Camera::Instance().SetView(XMFLOAT3(0.0f, 20.0f, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	Camera::Instance().SetProj(RADIAN(60.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 1.0f, 2000.0f);

	//Camera::Instance().SetView(XMFLOAT3(0.0f, 0.0f, -50000.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	//Camera::Instance().SetProj(RADIAN(60.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 1.0f, 200000.0f);

	/*EffectManager::Instance().SetView(&Camera::Instance().m_eye, &Camera::Instance().m_at, &Camera::Instance().m_up );
	EffectManager::Instance().SetProj(Camera::Instance().m_fov, Camera::Instance().m_aspect, Camera::Instance().m_zn, Camera::Instance().m_zf);*/

	//Create<TaskPlane>();
	Create<BilliardsTable>();
	/*Create<DrawFPS>();*/
}

Game::~Game()
{
}

bool Game::Run()
{
	timeCtrl->TimeRegular();	//フレーム制御
	timeCtrl->SetFPS(60);


	//TaskCall::All::Update(GROUP_UPDATE);
	//Task::All::Update();
	//EffectManager::Instance().Update();

	GameObjectManager::Update(CALL_TAG::TABLE);



	if (timeCtrl->GetDrawFlag())
	{
		//GameObjectManager::Draw(CALL_TAG::TABLE);
		DrawSystem::Instance().Draw();
	}
	
	return true;
}