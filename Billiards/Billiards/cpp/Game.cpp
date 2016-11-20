#include "../Header/Game.h"
#include "../Header/TimeControl.h"
#include "../Header/SpaceDivision.h"
#include "../Header/DrawSystem.h"
#include "../Header/Camera.h"
#include "../Header/GameObject.h"
#include "../Header/Collider.h"
#include "../Header/BilliardsTable.h"
//#include "../Header/Effect.h"

Game::Game()
{
	//Sprite::Create();
	DrawSystem::Instance().Init(1);

	timeCtrl = NEW TimeControl();

	SpaceDivision::Instance().Init(3,XMFLOAT3(-500.0f, -500.0f, -500.0f), XMFLOAT3(500.0f, 500.0f, 500.0f));

	Camera::Instance().SetView(XMFLOAT3(0.0f, 20.0f, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	Camera::Instance().SetProj(RADIAN(60.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 1.0f, 2000.0f);

	DrawSystem::Instance().SetView(&Camera::Instance().view);
	DrawSystem::Instance().SetProjection(&Camera::Instance().proj);

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
	SAFE_DELETE(timeCtrl);
}

bool Game::Run()
{
	timeCtrl->TimeRegular();	//ƒtƒŒ[ƒ€§Œä
	timeCtrl->SetFPS(60);


	//TaskCall::All::Update(GROUP_UPDATE);
	//Task::All::Update();
	//EffectManager::Instance().Update();

	GameObject::All::Update();
	Collider::All::HitCheck();


	if (timeCtrl->GetDrawFlag())
	{
		//GameObjectManager::Draw(CALL_TAG::TABLE);
		DrawSystem::Instance().Draw();
	}
	
	return true;
}