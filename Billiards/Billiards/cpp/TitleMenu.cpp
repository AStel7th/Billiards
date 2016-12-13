#include "../Header/TitleMenu.h"
#include "../Header/Game.h"
#include "../Header/Messenger.h"

TitleMenu::TitleMenu() : GameObject()
{
	titleTex = TextureContainer::Instance().LoadTexture(L"Resource/Texture/Billiards.png");
	titleSprite.InitCenter(&titleTex,200.0f,100.0f);
	titleSprite.SetScale(0.8f, 0.8f);

	startTex = TextureContainer::Instance().LoadTexture(L"Resource/Texture/Start.png");
	startOverTex = TextureContainer::Instance().LoadTexture(L"Resource/Texture/Start_3.png");
	startDownTex = TextureContainer::Instance().LoadTexture(L"Resource/Texture/Start_2.png");
	pStartButton = Create<Button<TitleMenu>>(400.0f, 300.0f, this,&TitleMenu::StartButtonDown,&startTex, &startOverTex, &startDownTex);

	exitTex = TextureContainer::Instance().LoadTexture(L"Resource/Texture/Exit.png");
	exitOverTex = TextureContainer::Instance().LoadTexture(L"Resource/Texture/Exit_3.png");
	exitDownTex = TextureContainer::Instance().LoadTexture(L"Resource/Texture/Exit_2.png");
	pExitButton = Create<Button<TitleMenu>>(400.0f, 400.0f, this, &TitleMenu::ExitButtonDown, &exitTex, &exitOverTex, &exitDownTex);

	Messenger::OnGamePhase.Add(*this, &TitleMenu::GoToGame);
}

TitleMenu::~TitleMenu()
{
	Messenger::OnGamePhase.Remove(*this, &TitleMenu::GoToGame);
}

void TitleMenu::Update()
{
	titleSprite.Draw();
}

void TitleMenu::StartButtonDown()
{
	Messenger::GameStart();
}

void TitleMenu::ExitButtonDown()
{
	Game::ExitGame();
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
	}
}