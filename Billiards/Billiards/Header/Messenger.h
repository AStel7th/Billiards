#pragma once
#include "Event.h"
#include "GameState.h"

class GameObject;

class Messenger
{
public:
	Messenger() {}

	virtual ~Messenger() {}

	static Event<void()> OnGameStart;

	static void GameStart()
	{
		if (OnGameStart.GetEventCount() != 0)
		{
			OnGameStart();
		}
	}

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

	static Event<void(GameObject*)> BallInPocket;

	static void SetBallInPocket(GameObject* pBall)
	{
		if (BallInPocket.GetEventCount() != 0)
		{
			BallInPocket(pBall);
		}
	}

	static Event<void()> isBallSetDone;

	static void BallSetDone()
	{
		if (isBallSetDone.GetEventCount() != 0)
		{
			isBallSetDone();
		}
	}
};