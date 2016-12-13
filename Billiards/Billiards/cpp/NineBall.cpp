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

NineBall::NineBall() : GameObject()
{
	SetName("NineBall");
	SetTag("NineBall");

	SetWorld();

	nowState = GAME_STATE::Title;

	Create<TitleMenu>();
	Create<House>();
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
	ballList[Create<Ball>(9, 0.0f, 78.0f, 0.0f)->name]	= false;		//TODO::�����ɐݒu����ƁA2�̃|���S������Փ˕��̔��˃x�N�g�����Ԃ��Ă��邽�ߏC������
	Create<CuesController>();
	Create<MainCamera>();

	Messenger::OnGameStart.Add(*this, &NineBall::GameStart);
	Messenger::OnShot.Add(*this, &NineBall::ShotPhase);
	Messenger::BallMovement.Add(*this, &NineBall::isBallMovement);
	Messenger::BallInPocket.Add(*this, &NineBall::IdentifyBall);
	Messenger::isBallSetDone.Add(*this, &NineBall::IdentifyBallSet);
}

NineBall::~NineBall()
{
	Messenger::OnGameStart.Remove(*this, &NineBall::GameStart);
	Messenger::OnShot.Remove(*this, &NineBall::ShotPhase);
	Messenger::BallMovement.Remove(*this, &NineBall::isBallMovement);
	Messenger::BallInPocket.Remove(*this, &NineBall::IdentifyBall);
	Messenger::isBallSetDone.Remove(*this, &NineBall::IdentifyBallSet);
}

void NineBall::Update()
{
	TCHAR s[256];

	switch (nowState)
	{
	case GAME_STATE::Title:
		_stprintf_s(s, _T("Title\n"));
		break;
	case GAME_STATE::Shot:
		_stprintf_s(s, _T("Shot\n"));
		break;
	case GAME_STATE::DecideOrder:
		_stprintf_s(s, _T("Decide\n"));
		break;
	case GAME_STATE::BallSet:
		_stprintf_s(s, _T("BallSet\n"));
		break;
	case GAME_STATE::BallMovement:
		_stprintf_s(s, _T("BallMovement\n"));
		break;
	}

	OutputDebugString(s);
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
	InputDeviceManager::Instance().CursorVisible(false);
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
		if (nowState != GAME_STATE::Title && nowState != GAME_STATE::Shot && nowState != GAME_STATE::BallSet)
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
