#pragma once
#include "GameObject.h"
#include "Sprite.h"
#include "GameState.h"
#include "Component.h"
#include "Direct3D11.h"

class TurnUIGraphics;
class WinUIGraphics;
class FoulUIGraphics;

class PlayerTurnUI : public GameObject
{
private:
	int player;
	
	enum WinOrTurn
	{
		None,
		Turn,
		FoulFromTurn,
		Win
	};

	WinOrTurn state;

	TurnUIGraphics* pTurnGraphics;
	WinUIGraphics* pWinGraphics;

	void TurnChange(GAME_STATE s, int num);
	void GameFinish(int num);
public:
	PlayerTurnUI(int playerNum);
	virtual ~PlayerTurnUI();

	void Update();
};

class TurnUIGraphics : public GraphicsComponent
{
private:
	TextureInfo turnTex;
	Sprite turnUI;

	int waitCnt;
	float moveSpeed;

	XMFLOAT2 initPos;
	
public:
	TurnUIGraphics(GameObject* pObj,int num);
	virtual ~TurnUIGraphics();

	bool Update();
};


class WinUIGraphics : public GraphicsComponent
{
private:
	TextureInfo winTex;
	Sprite winUI;

	int waitCnt;
	float moveSpeed;

	XMFLOAT2 initPos;

public:
	WinUIGraphics(GameObject* pObj, int num);
	virtual ~WinUIGraphics();

	bool Update();
};


class FoulUI : public GameObject
{
private:
	FoulUIGraphics* pGraphics;

	enum FoulState
	{
		None,
		InPocket,
		AnotherBall
	};

	FoulState state;

	void Foul(GAME_STATE state);
public:
	FoulUI();
	virtual ~FoulUI();

	void Update();
};

class FoulUIGraphics : public GraphicsComponent
{
private:
	TextureInfo foulTex;
	Sprite foulUI;

	int waitCnt;
	float moveSpeed;

	XMFLOAT2 initPos;

public:
	FoulUIGraphics(GameObject* pObj);
	virtual ~FoulUIGraphics();

	bool Update();
};