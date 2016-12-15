#include "../Header/NineBall.h"
#include "../Header/BilliardsTable.h"
#include "../Header/Ball.h"
#include "../Header/HandBall.h"
#include "../Header/Cues.h"
#include "../Header/Collider.h"
#include "../Header/Messenger.h"
#include "../Header/MainCamera.h"
#include "../Header/House.h"
#include "../Header/TitleMenu.h"
#include "../Header/InputDeviceManager.h"
#include "../Header/ResourceManager.h"
#include "../Header/AudioSystem.h"
#include "../Header/InGameUI.h"

NineBall::NineBall() : GameObject(), pBGM(nullptr), nextTargetBall(nullptr)
{
	SetName("NineBall");
	SetTag("NineBall");

	SetWorld();

	nowState = GAME_STATE::Title;

	Create<TitleMenu>();
	Create<House>();
	Create<BilliardsTable>();
	ballList[Create<HandBall>(61.5f, 78.0f, 0.0f)]	= false;
	ballList[Create<Ball>(1, 9.8f, 78.0f, 0.0f)]	= false;
	ballList[Create<Ball>(2, 0.0f, 78.0f, 5.8f)]	= false;
	ballList[Create<Ball>(3, -9.8f, 78.0f, 0.0f)]	= false;
	ballList[Create<Ball>(4, 0.0f, 78.0f, -5.8f)]	= false;
	ballList[Create<Ball>(5, 4.9f, 78.0f, -2.9f)]	= false;
	ballList[Create<Ball>(6, 4.9f, 78.0f, 2.9f)]	= false;
	ballList[Create<Ball>(7, -4.9f, 78.0f, -2.9f)] = false;
	ballList[Create<Ball>(8, -4.9f, 78.0f, 2.9f)]	= false;
	ballList[Create<Ball>(9, 0.0f, 78.0f, 0.0f)]	= false;		//TODO::中央に設置すると、2つのポリゴンから衝突分の反射ベクトルが返ってくるため修正する
	Create<CuesController>();
	Create<MainCamera>();
	Create<PlayerTurnUI>(1);
	Create<PlayerTurnUI>(2);
	Create<FoulUI>();

	WaveFileLoader* data;
	ResourceManager::Instance().GetResource(&data, "GameBGM", "Resource/Sound/BGM.wav");
	pBGM = NEW SoundPlayer();
	pBGM->Create(data, true);
	pBGM->Play();

	playerTurn = 1;

	nextTargetBall = GameObject::All::GameObjectFindWithName("Ball1");

	isFirstHit = false;

	Messenger::OnGameStart.Add(*this, &NineBall::GameStart);
	Messenger::OnGameStateRequest.Add(*this, &NineBall::GameStateRequest);
	Messenger::BallMovement.Add(*this, &NineBall::isBallMovement);
	Messenger::BallInPocket.Add(*this, &NineBall::IdentifyBall);
	Messenger::FirstHitBall.Add(*this, &NineBall::FirstHit);
}

NineBall::~NineBall()
{
	pBGM->Stop();
	SAFE_DELETE(pBGM);

	Messenger::OnGameStart.Remove(*this, &NineBall::GameStart);
	Messenger::OnGameStateRequest.Remove(*this, &NineBall::GameStateRequest);
	Messenger::BallMovement.Remove(*this, &NineBall::isBallMovement);
	Messenger::BallInPocket.Remove(*this, &NineBall::IdentifyBall);
	Messenger::FirstHitBall.Remove(*this, &NineBall::FirstHit);
}

void NineBall::Update()
{

}

bool NineBall::IsBallMoving()
{
	for (auto&& elem : ballList)
	{
		if (elem.second == true)
			return true;
	}

	return false;
}

void NineBall::GameStart()
{
	nowState = GAME_STATE::DecideOrder;
	Messenger::GamePhase(nowState);
	Messenger::TurnChange(nowState,playerTurn);
	InputDeviceManager::Instance().CursorVisible(false);
}

void NineBall::GameStateRequest(GAME_STATE state)
{
	nowState = state;
	Messenger::GamePhase(nowState);

	if (state == GAME_STATE::FoulFromDecide || state == GAME_STATE::DecideOrder)
	{
		if (playerTurn > 1)
			playerTurn = 1;
		else
			playerTurn = 2;

		Messenger::TurnChange(nowState,playerTurn);
	}
}

//ボールが動いているか確認
void NineBall::isBallMovement(GameObject * pBall,bool flg)
{
	auto it = ballList.find(pBall);

	if (it != ballList.end())
	{
		it->second = flg;
	}

	if (!IsBallMoving())
	{
		if (nowState == GAME_STATE::BallMovement)
		{
			if (!isFirstHit)
			{
				nowState = GAME_STATE::DecideOrder;
				Messenger::GamePhase(nowState);

				if (playerTurn > 1)
					playerTurn = 1;
				else
					playerTurn = 2;

				Messenger::TurnChange(nowState, playerTurn);
			}
			else
			{
				nowState = GAME_STATE::Shot;
				Messenger::GamePhase(nowState);
			}

			if(!nextTargetBall->isActive())
			{
				for each (pair<GameObject*, bool> var in ballList)
				{
					if (var.second == true)
					{
						nextTargetBall = var.first;
					}
				}
			}

			isFirstHit = false;
		}
	}
	else
	{
		if (nowState != GAME_STATE::BallMovement)
		{
			nowState = GAME_STATE::BallMovement;
			Messenger::GamePhase(nowState);
		}
	}
}

//ポケットに入ったボール確認
void NineBall::IdentifyBall(GameObject * pBall)
{
	if (pBall->name == "HandBall")
	{
		nowState = GAME_STATE::FoulInPocket;
		Messenger::GamePhase(nowState);
	}

	if (pBall->name == "Ball9")
	{
		Messenger::GameFinish(playerTurn);
	}
}

//最初にヒットしたボール確認
void NineBall::FirstHit(GameObject * pBall)
{
	isFirstHit = true;

	if (pBall->name != nextTargetBall->name)
	{
		nowState = GAME_STATE::FoulAnotherBall;
		Messenger::GamePhase(nowState);
	}
}
