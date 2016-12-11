#include "../Header/NineBall.h"
#include "../Header/BilliardsTable.h"
#include "../Header/Ball.h"
#include "../Header/HandBall.h"
#include "../Header/Cues.h"
#include "../Header/Collider.h"
#include "../Header/Messenger.h"
#include "../Header/MainCamera.h"

NineBall::NineBall() : GameObject()
{
	SetName("NineBall");
	SetTag("NineBall");

	SetWorld();

	Create<BilliardsTable>();
	ballList[Create<HandBall>(61.5f, 78.0f, 0.0f)->name]	= false;
	ballList[Create<Ball>(1, 9.8f, 78.0f, 0.0f)->name]	= false;
	ballList[Create<Ball>(2, 0.0f, 78.0f, 5.8f)->name]	= false;
	ballList[Create<Ball>(3, -9.8f, 78.0f, 0.0f)->name]	= false;
	ballList[Create<Ball>(4, 0.0f, 78.0f, -5.8f)->name]	= false;
	ballList[Create<Ball>(5, 4.9f, 78.0f, -2.9f)->name]	= false;
	ballList[Create<Ball>(6, 4.9f, 78.0f, 2.9f)->name]	= false;
	ballList[Create<Ball>(7, -4.9f, 78.0f, -2.9f)->name] = false;
	ballList[Create<Ball>(8, -4.9f, 78.0f, 2.9f)->name]	= false;
	//ballList[Create<Ball>(9, 0.0f, 78.0f, 0.0f)->name]	= false;		//TODO::中央に設置すると、2つのポリゴンから衝突分の反射ベクトルが返ってくるため修正する
	Create<CuesController>();
	Create<MainCamera>();

	nowState = GAME_STATE::DecideOrder;

	Messenger::OnShot.Add(*this, &NineBall::ShotPhase);
	Messenger::BallMovement.Add(*this, &NineBall::isBallMovement);
	Messenger::BallInPocket.Add(*this, &NineBall::IdentifyBall);
	Messenger::isBallSetDone.Add(*this, &NineBall::IdentifyBallSet);
}

NineBall::~NineBall()
{
	Messenger::OnShot.Remove(*this, &NineBall::ShotPhase);
	Messenger::BallMovement.Remove(*this, &NineBall::isBallMovement);
	Messenger::BallInPocket.Remove(*this, &NineBall::IdentifyBall);
	Messenger::isBallSetDone.Remove(*this, &NineBall::IdentifyBallSet);
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

void NineBall::ShotPhase()
{
	/*nowState = GAME_STATE::BallMovement;
	Messenger::GamePhase(nowState);*/
}


void NineBall::isBallMovement(GameObject * pBall,bool flg)
{
	auto it = ballList.find(pBall->name);

	if (it != ballList.end())
	{
		it->second = flg;
	}

	if (!IsBallMoving())
	{
		if (nowState != GAME_STATE::Shot && nowState != GAME_STATE::BallSet)
		{
			nowState = GAME_STATE::Shot;
			Messenger::GamePhase(nowState);
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

void NineBall::IdentifyBall(GameObject * pBall)
{
	if (pBall->name == "HandBall")
	{
		nowState = GAME_STATE::BallSet;
		Messenger::GamePhase(nowState);
	}
}

void NineBall::IdentifyBallSet()
{
	nowState = GAME_STATE::Shot;
	Messenger::GamePhase(nowState);
}
