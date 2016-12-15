#include "../Header/TitleMenu.h"
#include "../Header/Game.h"
#include "../Header/Messenger.h"
#include "../Header/InputDeviceManager.h"
#include "../Header/AudioSystem.h"
#include "../Header/ResourceManager.h"

TitleMenu::TitleMenu() : GameObject(), pClickSE(nullptr), marginCnt(0), sceneChangeFlg(0)
{
	titleTex = TextureContainer::Instance().LoadTexture("Resource/Texture/Billiards.png");
	titleSprite.InitCenter(&titleTex,200.0f,100.0f);
	titleSprite.SetScale(0.8f, 0.8f);

	startTex = TextureContainer::Instance().LoadTexture("Resource/Texture/Start.png");
	startOverTex = TextureContainer::Instance().LoadTexture("Resource/Texture/Start_3.png");
	startDownTex = TextureContainer::Instance().LoadTexture("Resource/Texture/Start_2.png");
	pStartButton = Create<Button<TitleMenu>>(450.0f, 250.0f, this,&TitleMenu::StartButtonDown,&startTex, &startOverTex, &startDownTex);

	exitTex = TextureContainer::Instance().LoadTexture("Resource/Texture/Exit.png");
	exitOverTex = TextureContainer::Instance().LoadTexture("Resource/Texture/Exit_3.png");
	exitDownTex = TextureContainer::Instance().LoadTexture("Resource/Texture/Exit_2.png");
	pExitButton = Create<Button<TitleMenu>>(450.0f, 340.0f, this, &TitleMenu::ExitButtonDown, &exitTex, &exitOverTex, &exitDownTex);

	WaveFileLoader* data;
	ResourceManager::Instance().GetResource(&data, "ClickSE", "Resource/Sound/Click.wav");

	pClickSE = NEW SoundPlayer();
	pClickSE->Create(data);

	Messenger::OnGamePhase.Add(*this, &TitleMenu::GoToGame);
}

TitleMenu::~TitleMenu()
{
	SAFE_DELETE(pClickSE);
	Messenger::OnGamePhase.Remove(*this, &TitleMenu::GoToGame);
}

void TitleMenu::Update()
{
	if (sceneChangeFlg != 0)
	{
		marginCnt++;

		if (marginCnt > 30)
		{
			if(sceneChangeFlg == 1)
				Messenger::GameStart();
			else if(sceneChangeFlg == 2)
				Game::ExitGame();

			marginCnt = 0;
			sceneChangeFlg = false;
		}
	}

	titleSprite.Draw();
}

void TitleMenu::StartButtonDown()
{
	pClickSE->Play();
	sceneChangeFlg = 1;
}

void TitleMenu::ExitButtonDown()
{
	pClickSE->Play();
	sceneChangeFlg = 2;
}

void TitleMenu::GoToGame(GAME_STATE state)
{
	if (state != GAME_STATE::Title)
	{
		pStartButton->SetActive(false);
		pExitButton->SetActive(false);
		this->SetActive(false);
	}
	else
	{
		pStartButton->SetActive(true);
		pExitButton->SetActive(true);
		this->SetActive(true);
		InputDeviceManager::Instance().CursorVisible(true);
	}
}