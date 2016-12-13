#pragma once
#include "GameObject.h"
#include "GameState.h"
#include <map>

using namespace std;

class NineBall : public GameObject
{
private:
	GAME_STATE nowState;
	map<string, bool> ballList;

	void GameStart();

	void ShotPhase();

	bool IsBallMoving();

	void isBallMovement(GameObject* pBall, bool flg);

	void IdentifyBall(GameObject* pBall);

	void IdentifyBallSet();
public:
	NineBall();
	virtual ~NineBall();

	void Update();
};