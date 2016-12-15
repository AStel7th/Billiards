#pragma once
#include "GameObject.h"
#include "GameState.h"
#include <map>

using namespace std;

class SoundPlayer;

class NineBall : public GameObject
{
private:
	GAME_STATE nowState;
	map<GameObject*, bool> ballList;
	SoundPlayer* pBGM;
	GameObject* nextTargetBall;
	bool isFirstHit;

	int playerTurn;

	void GameStart();

	void GameStateRequest(GAME_STATE state);

	bool IsBallMoving();

	void isBallMovement(GameObject* pBall, bool flg);

	void IdentifyBall(GameObject* pBall);

	void FirstHit(GameObject* pBall);
public:
	NineBall();
	virtual ~NineBall();

	void Update();
};