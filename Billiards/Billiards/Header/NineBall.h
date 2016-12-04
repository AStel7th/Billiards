#pragma once
#include "GameObject.h"
#include "Messenger.h"
#include <map>

using namespace std;

class NineBall : public GameObject
{
private:
	GAME_STATE nowState;
	map<string, bool> ballList;
public:
	NineBall();
	virtual ~NineBall();

	void Update();

	void ShotPhase();

	bool IsBallMoving();

	void isBallMovement(GameObject* pBall,bool flg);
};