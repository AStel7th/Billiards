#include "../Header/InGameUI.h"
#include "../Header/Messenger.h"

PlayerTurnUI::PlayerTurnUI(int playerNum) : GameObject(), pTurnGraphics(nullptr),pWinGraphics(nullptr)
{
	player = playerNum;

	SetName("PlayerTurnUI" + to_string(player));
	SetTag("PlayerTurnUI");

	SetWorld();

	pTurnGraphics = NEW TurnUIGraphics(this,player);
	pWinGraphics = NEW WinUIGraphics(this, player);

	state = None;

	Messenger::OnTurnChange.Add(*this, &PlayerTurnUI::TurnChange);
	Messenger::OnGameFinish.Add(*this, &PlayerTurnUI::GameFinish);
}

PlayerTurnUI::~PlayerTurnUI()
{
	SAFE_DELETE(pTurnGraphics);
	SAFE_DELETE(pWinGraphics);
	Messenger::OnTurnChange.Remove(*this, &PlayerTurnUI::TurnChange);
	Messenger::OnGameFinish.Remove(*this, &PlayerTurnUI::GameFinish);
}

void PlayerTurnUI::Update()
{
	switch (state)
	{
	case WinOrTurn::Turn:
		if (!pTurnGraphics->Update())
		{
			state = None;
			Messenger::GameStateRequest(GAME_STATE::Shot);
		}
		break;

	case WinOrTurn::FoulFromTurn:
		if (!pTurnGraphics->Update())
		{
			state = None;
			Messenger::GameStateRequest(GAME_STATE::BallSet);
		}
		break;
	case WinOrTurn::Win:
		if (!pWinGraphics->Update())
		{
			state = None;
			Messenger::GameStateRequest(GAME_STATE::Title);
		}
		break;
	default:
		break;
	}
}


void PlayerTurnUI::TurnChange(GAME_STATE s,int num)
{
	if (num == player)
	{
		switch (s)
		{
		case GAME_STATE::DecideOrder:
			state = Turn;
			break;

		case GAME_STATE::FoulFromDecide:
			state = FoulFromTurn;
			break;
		}
		
	}
}

void PlayerTurnUI::GameFinish(int num)
{
	if (num == player)
	{
		state = Win;
	}
}


TurnUIGraphics::TurnUIGraphics(GameObject* pObj, int num) : GraphicsComponent()
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	initPos = XMFLOAT2((float)WINDOW_WIDTH * 3.5f, (float)WINDOW_HEIGHT / 2.0f);

	waitCnt = 0;
	moveSpeed = 8.0f;

	turnTex = TextureContainer::Instance().LoadTexture("Resource/Texture/Player" + to_string(num) + "-turn.png");
	turnUI.InitCenter(&turnTex, initPos.x, initPos.y);
}

TurnUIGraphics::~TurnUIGraphics()
{
}

bool TurnUIGraphics::Update()
{
	XMFLOAT2 nowPos = *turnUI.GetPos();

	if (nowPos.x <= (float)WINDOW_WIDTH / 2.0f && waitCnt < 60)
	{
		turnUI.SetPos(nowPos.x, nowPos.y);
		waitCnt++;
	}
	else
	{
		turnUI.SetPos(nowPos.x - moveSpeed, nowPos.y);
	}

	if (nowPos.x < -turnUI.GetSize().x)
	{
		waitCnt = 0;
		turnUI.SetPos(initPos.x, initPos.y);
		return false;
	}

	turnUI.Draw();

	return true;
}


WinUIGraphics::WinUIGraphics(GameObject* pObj, int num) : GraphicsComponent()
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	initPos = XMFLOAT2((float)WINDOW_WIDTH * 3.5f, (float)WINDOW_HEIGHT / 2.0f);

	waitCnt = 0;
	moveSpeed = 8.0f;

	winTex = TextureContainer::Instance().LoadTexture("Resource/Texture/Player" + to_string(num) + "-Win.png");
	winUI.InitCenter(&winTex, initPos.x, initPos.y);
}

WinUIGraphics::~WinUIGraphics()
{
}

bool WinUIGraphics::Update()
{
	XMFLOAT2 nowPos = *winUI.GetPos();

	if (nowPos.x <= (float)WINDOW_WIDTH / 2.0f && waitCnt < 60)
	{
		winUI.SetPos(nowPos.x, nowPos.y);
		waitCnt++;
	}
	else
	{
		winUI.SetPos(nowPos.x - moveSpeed, nowPos.y);
	}

	if (nowPos.x < -winUI.GetSize().x)
	{
		waitCnt = 0;
		winUI.SetPos(initPos.x, initPos.y);
		return false;
	}

	winUI.Draw();

	return true;
}


FoulUI::FoulUI() : GameObject(), pGraphics(nullptr)
{
	SetName("FoulUI");
	SetTag("FoulUI");

	SetWorld();

	pGraphics = NEW FoulUIGraphics(this);

	state = None;

	Messenger::OnGamePhase.Add(*this, &FoulUI::Foul);
}

FoulUI::~FoulUI()
{
	SAFE_DELETE(pGraphics);
	Messenger::OnGamePhase.Remove(*this, &FoulUI::Foul);
}

void FoulUI::Update()
{
	switch(state)
	{
	case FoulState::AnotherBall:

		if (!pGraphics->Update())
		{
			Messenger::GameStateRequest(GAME_STATE::DecideOrder);
			state = None;
		}
		break;

	case FoulState::InPocket:

		if (!pGraphics->Update())
		{
			Messenger::GameStateRequest(GAME_STATE::FoulFromDecide);
			state = None;
		}
		break;
	}
}

void FoulUI::Foul(GAME_STATE s)
{
	if(s == GAME_STATE::FoulInPocket)
		state = InPocket;
	else if(s == GAME_STATE::FoulAnotherBall)
		state = AnotherBall;
}

FoulUIGraphics::FoulUIGraphics(GameObject * pObj)
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	initPos = XMFLOAT2((float)WINDOW_WIDTH * 1.5f, (float)WINDOW_HEIGHT / 2.0f);

	waitCnt = 0;
	moveSpeed = 8.0f;

	foulTex = TextureContainer::Instance().LoadTexture("Resource/Texture/Foul.png");
	foulUI.InitCenter(&foulTex, initPos.x, initPos.y);
}

FoulUIGraphics::~FoulUIGraphics()
{
}

bool FoulUIGraphics::Update()
{
	XMFLOAT2 nowPos = *foulUI.GetPos();

	if (nowPos.x <= (float)WINDOW_WIDTH / 2.0f && waitCnt < 60)
	{
		foulUI.SetPos(nowPos.x, nowPos.y);
		waitCnt++;
	}
	else
	{
		foulUI.SetPos(nowPos.x - moveSpeed, nowPos.y);
	}

	if (nowPos.x < -foulUI.GetSize().x)
	{
		waitCnt = 0;
		foulUI.SetPos(initPos.x, initPos.y);
		return false;
	}

	foulUI.Draw();

	return true;
}
