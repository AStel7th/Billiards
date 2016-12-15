#include "../Header/MainCamera.h"
#include "../Header/Camera.h"
#include "../Header/Direct3D11.h"
#include "../Header/Messenger.h"

MainCamera::MainCamera() : GameObject()
{
	SetName("MainCamera");

	SetTag("MainCamera");

	pInputComponent = NEW MainCameraInput(this);

	Camera::Instance().SetView(XMFLOAT3(-600.0f, 150.0f, 200.0f), XMFLOAT3(200.0f, 200.0f, -400.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	Camera::Instance().SetProj(RADIAN(60.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 1.0f, 2000.0f);
}

MainCamera::~MainCamera()
{
	SAFE_DELETE(pInputComponent);
}

void MainCamera::Update()
{
	pInputComponent->Update();
}


MainCameraInput::MainCameraInput(GameObject * pObj) : InputComponent(), whiteBall(nullptr), cues(nullptr), moveSpeed(0.0f)
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	whiteBall = GameObject::All::GameObjectFindWithName("HandBall");
	cues = GameObject::All::GameObjectFindWithName("Cues");

	camState = GAME_STATE::Title;

	moveSpeed = 5.0f;

	startPos = XMFLOAT3(-600.0f, 150.0f, 200.0f);
	startAt = XMFLOAT3(200.0f, 200.0f, -400.0f);
	followStatePos = XMFLOAT3(0.0f, 300.0f, -150.0f);

	Messenger::OnGamePhase.Add(*this, &MainCameraInput::GamePhase);
}

MainCameraInput::~MainCameraInput()
{

}

bool MainCameraInput::Update()
{
	XMFLOAT3 cameraPos;
	XMFLOAT3 cameraAt;

	switch (camState)
	{
	case GAME_STATE::Title:
	{
		XMFLOAT3 nowPos = XMFLOAT3(Camera::Instance().m_eye.x, Camera::Instance().m_eye.y, Camera::Instance().m_eye.z);
		XMFLOAT3 nowAt = XMFLOAT3(Camera::Instance().m_at.x, Camera::Instance().m_at.y, Camera::Instance().m_at.z);
		MovePosition(nowPos, startPos, cameraPos);
		MovePosition(nowAt, startAt, cameraAt);
	}
		break;
	case GAME_STATE::FoulFromDecide:
	case GAME_STATE::DecideOrder:
	{
		XMFLOAT3 nowPos = XMFLOAT3(Camera::Instance().m_eye.x, Camera::Instance().m_eye.y, Camera::Instance().m_eye.z);
		XMFLOAT3 nowAt = XMFLOAT3(Camera::Instance().m_at.x, Camera::Instance().m_at.y, Camera::Instance().m_at.z);
		MovePosition(nowPos, followStatePos, cameraPos);
		MovePosition(nowAt, XMFLOAT3(0.0f,0.0f,0.0f), cameraAt);
	}

		break;
	case GAME_STATE::Shot:
	{
		XMFLOAT3 nowPos = XMFLOAT3(Camera::Instance().m_eye.x, Camera::Instance().m_eye.y, Camera::Instance().m_eye.z);
		XMVECTOR distVec;
		XMVECTOR pos;
		XMVECTOR cuesPos = XMLoadFloat3(&cues->GetWorldPos());
		XMVECTOR ballPos = XMLoadFloat3(&whiteBall->GetWorldPos());

		distVec = ballPos - cuesPos;
		distVec = XMVector3Normalize(distVec);

		pos = ballPos - distVec * 120.0f;

		XMStoreFloat3(&cameraPos, pos);

		cameraPos.y = 110.0f;

		MovePosition(nowPos,cameraPos, cameraPos);

		cameraAt = whiteBall->pos;
	}
	break;
	case GAME_STATE::BallMovement:
	{
		XMFLOAT3 nowPos = XMFLOAT3(Camera::Instance().m_eye.x, Camera::Instance().m_eye.y, Camera::Instance().m_eye.z);
		MovePosition(nowPos, followStatePos, cameraPos);
		cameraAt = whiteBall->pos;
	}
		break;
	case GAME_STATE::BallSet:
	{
		XMFLOAT3 nowPos = XMFLOAT3(Camera::Instance().m_eye.x, Camera::Instance().m_eye.y, Camera::Instance().m_eye.z);
		MovePosition(nowPos,followStatePos, cameraPos);
		cameraAt = whiteBall->pos;
	}
		
		break;

	case GAME_STATE::FoulInPocket:
	case GAME_STATE::FoulAnotherBall:
	{
		XMFLOAT3 nowPos = XMFLOAT3(Camera::Instance().m_eye.x, Camera::Instance().m_eye.y, Camera::Instance().m_eye.z);
		XMFLOAT3 nowAt = XMFLOAT3(Camera::Instance().m_at.x, Camera::Instance().m_at.y, Camera::Instance().m_at.z);
		MovePosition(nowPos, followStatePos, cameraPos);
		MovePosition(nowAt, XMFLOAT3(0.0f, 0.0f, 0.0f), cameraAt);
	}

	break;
	}

	Camera::Instance().SetView(cameraPos, cameraAt, XMFLOAT3(0.0f, 1.0f, 0.0f));

	return true;
}

void MainCameraInput::MovePosition(XMFLOAT3& nPos,XMFLOAT3& goal, XMFLOAT3& outPos)
{
	XMVECTOR nowPos = XMLoadFloat3(&nPos);
	XMVECTOR goalPos = XMLoadFloat3(&goal);
	XMVECTOR distVec = goalPos - nowPos;
	float distance;
	float moveProportion = 1.0f;

	XMStoreFloat(&distance, XMVector3Length(distVec));

	distVec = XMVector3Normalize(distVec);

	if (distance < moveSpeed)
		moveProportion = distance / moveSpeed;

	nowPos = nowPos + distVec * (moveSpeed * moveProportion);

	XMStoreFloat3(&outPos, nowPos);
}

void MainCameraInput::GamePhase(GAME_STATE state)
{
	camState = state;
}
