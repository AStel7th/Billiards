#include "../Header/MainCamera.h"
#include "../Header/Camera.h"
#include "../Header/Direct3D11.h"
#include "../Header/Messenger.h"

MainCamera::MainCamera() : GameObject()
{
	SetName("MainCamera");

	SetTag("MainCamera");

	pInputComponent = NEW MainCameraInput(this);

	Camera::Instance().SetView(XMFLOAT3(0.0f, 300.0f, -150.0f), XMFLOAT3(0.0f, 40.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
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

	camState = Shot;

	moveSpeed = 3.0f;

	followStatePos = XMFLOAT3(0.0f, 300.0f, -150.0f);

	Messenger::OnGamePhase.Add(*this, &MainCameraInput::GamePhase);
}

MainCameraInput::~MainCameraInput()
{

}

void MainCameraInput::Update()
{
	XMFLOAT3 cameraOut;

	switch (camState)
	{
	case CameraState::Shot:
	{
		XMVECTOR distVec;
		XMVECTOR cameraPos;
		XMVECTOR cuesPos = XMLoadFloat3(&cues->GetWorldPos());
		XMVECTOR ballPos = XMLoadFloat3(&whiteBall->GetWorldPos());

		distVec = ballPos - cuesPos;
		distVec = XMVector3Normalize(distVec);

		cameraPos = ballPos - distVec * 120.0f;

		XMStoreFloat3(&cameraOut, cameraPos);

		cameraOut.y = 110.0f;

		MovePosition(cameraOut, cameraOut);
	}
		break;
	case CameraState::FollowMovement:
		MovePosition(followStatePos, cameraOut);
		break;
	case CameraState::BallSet:
		MovePosition(followStatePos, cameraOut);
		break;
	}

	Camera::Instance().SetView(cameraOut, whiteBall->GetWorldPos(), XMFLOAT3(0.0f, 1.0f, 0.0f));
}

void MainCameraInput::MovePosition(XMFLOAT3& goal, XMFLOAT3& outPos)
{
	XMVECTOR nowPos = XMVectorSet(Camera::Instance().m_eye.x, Camera::Instance().m_eye.y, Camera::Instance().m_eye.z, 1.0f);
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
	switch (state)
	{
	case GAME_STATE::Shot:

		camState = Shot;

		break;

	case GAME_STATE::BallMovement:

		camState = FollowMovement;

		break;

	case GAME_STATE::BallSet:

		camState = BallSet;

		break;
	}
}
