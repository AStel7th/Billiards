#pragma once
#include "Event.h"

class GameObject;


enum struct GAME_STATE
{
	DecideOrder,
	BallMovement,
};

class Messenger
{
public:
	Messenger() {}

	virtual ~Messenger() {}

	static Event<void(GAME_STATE)> OnGamePhase;

	static void GamePhase(GAME_STATE state)
	{
		if (OnGamePhase.GetEventCount() != 0)
		{
			OnGamePhase(state);
		}
	}

	static Event<void()> OnShot;

	static void Shot()
	{
		if (OnShot.GetEventCount() != 0)
		{
			OnShot();
		}
	}

	static Event<void(GameObject*,bool)> BallMovement;

	static void isBallMovement(GameObject* pBall,bool flg)
	{
		if (BallMovement.GetEventCount() != 0)
		{
			BallMovement(pBall,flg);
		}
	}
};