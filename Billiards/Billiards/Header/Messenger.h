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

	static Event<void(GAME_STATE , int)> OnTurnChange;

	static void TurnChange(GAME_STATE s, int playerNum)
	{
		if (OnTurnChange.GetEventCount() != 0)
		{
			OnTurnChange(s,playerNum);
		}
	}

	static Event<void(int)> OnGameFinish;

	static void GameFinish(int playerNum)
	{
		if (OnGameFinish.GetEventCount() != 0)
		{
			OnGameFinish(playerNum);
		}
	}

	static Event<void(GAME_STATE)> OnGameStateRequest;

	static void GameStateRequest(GAME_STATE state)
	{
		if (OnGameStateRequest.GetEventCount() != 0)
		{
			OnGameStateRequest(state);
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

	static Event<void(GameObject*)> FirstHitBall;

	static void SetFirstHitBall(GameObject* pBall)
	{
		if (FirstHitBall.GetEventCount() != 0)
		{
			FirstHitBall(pBall);
		}
	}
};