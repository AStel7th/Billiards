#pragma once
#include "GameObject.h"
#include "Sprite.h"
#include "Button.h"	
#include "GameState.h"

class SoundPlayer;

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

	SoundPlayer* pClickSE;

	int marginCnt;
	int sceneChangeFlg;

	void StartButtonDown();
	void ExitButtonDown();

	void GoToGame(GAME_STATE state);
public:
	TitleMenu();
	virtual ~TitleMenu();

	void Update();
};