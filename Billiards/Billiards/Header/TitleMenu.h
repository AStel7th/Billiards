#pragma once
#include "GameObject.h"
#include "Sprite.h"
#include "Button.h"	
#include "GameState.h"

class TitleMenu : public GameObject
{
private:
	TextureInfo titleTex;
	Sprite titleSprite;

	TextureInfo startTex;
	TextureInfo startOverTex;
	TextureInfo startDownTex;
	TextureInfo exitTex;
	TextureInfo exitOverTex;
	TextureInfo exitDownTex;

	Button<TitleMenu>* pStartButton;
	Button<TitleMenu>* pExitButton;

	void StartButtonDown();
	void ExitButtonDown();

	void GoToGame(GAME_STATE state);
public:
	TitleMenu();
	virtual ~TitleMenu();

	void Update();
};