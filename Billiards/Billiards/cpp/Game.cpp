#include "../Header/Game.h"
#include "../Header/TimeControl.h"
#include "../Header/AudioSystem.h"
#include "../Header/SpaceDivision.h"
#include "../Header/DrawSystem.h"
#include "../Header/Camera.h"
#include "../Header/GameObject.h"
#include "../Header/Collider.h"
#include "../Header/NineBall.h"
#include "../Header/InputDeviceManager.h"
//#include "../Header/Effect.h"

bool Game::isExit = false;

Game::Game()
{
	// �J�[�\���̓���͈͂𐧌�����
	InputDeviceManager::Instance().ClipCursorWindow(true);

	AudioSystem::Instance().Create();

	Sprite::Create();
	DrawSystem::Instance().Init(1);

	timeCtrl = NEW TimeControl();

	SpaceDivision::Instance().Init(5,XMFLOAT3(-250.0f, -250.0f, -250.0f), XMFLOAT3(250.0f, 250.0f, 250.0f));

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
	timeCtrl->TimeRegular();	//�t���[������
	timeCtrl->SetFPS(60);

	//EffectManager::Instance().Update();
	GameObject::All::Update();
	Collider::All::HitCheck();


	if (timeCtrl->GetDrawFlag())
	{
		DrawSystem::Instance().Draw();
	}
	
	if (isExit)
		return false;
	else
		return true;
}