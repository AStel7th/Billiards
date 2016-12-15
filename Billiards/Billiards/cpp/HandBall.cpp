#include "../Header/HandBall.h"
#include "../Header/ResourceManager.h"
#include "../Header/Collider.h"
#include "../Header/BallComponents.h"
#include "../Header/Messenger.h"
#include "../Header/InputDeviceManager.h"

HandBall::HandBall(float x, float y, float z) : GameObject(), 
pMesh(nullptr), 
pCollider(nullptr), 
pInputComponent(nullptr),
pPhysicsComponent(nullptr), 
pGraphicsComponent(nullptr)
{
	SetName("HandBall");

	SetTag("Ball");

	SetLayer("Ball");

	ResourceManager::Instance().GetResource(&pMesh,"Ball" + to_string(0), "Resource/Ball.fbx");

	pPhysicsComponent = NEW BallPhysics(this);

	pGraphicsComponent = NEW BallGraphics(this, pMesh, 0);
	
	pInputComponent = NEW HandBallInput(this);

	pCollider = NEW SphereCollider();
	pCollider->Create(this, Sphere, 2.6f);

	pos.x = x;
	pos.y = y;
	pos.z = z;

	SetWorld();
}

HandBall::~HandBall()
{
	pCollider->Destroy();
	SAFE_DELETE(pInputComponent);
	SAFE_DELETE(pPhysicsComponent);
	SAFE_DELETE(pGraphicsComponent);
}

void HandBall::Update()
{
	pInputComponent->Update();

	pPhysicsComponent->Update();

	pGraphicsComponent->Update();

	if(!pInputComponent->setFlag)
		pCollider->Update();
}

HandBallInput::HandBallInput(GameObject * pObj) : InputComponent(), setFlag(false), handBallPhysics(nullptr)
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	handBallPhysics = GetComponent<PhysicsComponent>(pGameObject);

	Messenger::OnGamePhase.Add(*this, &HandBallInput::GamePhase);
}

HandBallInput::~HandBallInput()
{
}

bool HandBallInput::Update()
{
	//ファール時の再設置
	if (setFlag)
	{
		handBallPhysics->velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

		float moveX = InputDeviceManager::Instance().GetMouseState().x / 10.0f;
		float moveZ = InputDeviceManager::Instance().GetMouseState().y / 10.0f;

		if (pGameObject->pos.x + moveX < -125.0f || pGameObject->pos.x + moveX > 125.0f)
		{
			pGameObject->pos.x += 0.0f;
		}
		else
		{
			pGameObject->pos.x += InputDeviceManager::Instance().GetMouseState().x / 10.0f;
		}

		pGameObject->pos.y = 78.0f;

		if (pGameObject->pos.z - moveZ < -65.0f || pGameObject->pos.z - moveZ > 65.0f)
		{
			pGameObject->pos.z += 0.0f;
		}
		else
		{
			pGameObject->pos.z -= InputDeviceManager::Instance().GetMouseState().y / 10.0f;
		}

		if (InputDeviceManager::Instance().GetMouseButtonDown(0) == true)
			BallSetDone();
	}

	return true;
}

void HandBallInput::GamePhase(GAME_STATE state)
{
	if (state == GAME_STATE::BallSet)
	{
		setFlag = true;

		pGameObject->pos = XMFLOAT3(0.0f, 80.0f, 0.0f);
	}
}

void HandBallInput::BallSetDone()
{
	setFlag = false;
	Messenger::GameStateRequest(GAME_STATE::Shot);
}
