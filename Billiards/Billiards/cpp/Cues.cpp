#include "../Header/Cues.h"
#include "../Header/ResourceManager.h"
#include "../Header/Collider.h"
#include "../Header/CuesComponents.h"
#include "../Header/Messenger.h"
#include "../Header/InputDeviceManager.h"

CuesController::CuesController() : GameObject(), pInputComponent(nullptr),whiteBall(nullptr)
{
	SetName("CuesController");

	SetTag("CuesController");

	SetLayer("Cues");
	
	whiteBall = GameObject::All::GameObjectFindWithName("HandBall");

	pos = whiteBall->pos;

	pInputComponent = NEW CuesControllerInput(this);

	SetWorld();
	
	AddChild(Create<Cues>());

	Messenger::OnGamePhase.Add(*this, &CuesController::ShotPhase);
}

CuesController::~CuesController()
{
	Messenger::OnGamePhase.Remove(*this, &CuesController::ShotPhase);
	SAFE_DELETE(pInputComponent);
}

void CuesController::Update()
{
	pInputComponent->Update();
}

void CuesController::ShotPhase(GAME_STATE state)
{
	if (state == GAME_STATE::Shot)
	{
		pos = whiteBall->pos;
		
		SetWorld();
		
		SetActive(true);
	}
	else if (state == GAME_STATE::BallSet)
	{
		SetActive(false);
	}
}

Cues::Cues() : GameObject(),pMesh(nullptr),pCollider(nullptr),pInputComponent(nullptr),pPhysicsComponent(nullptr),pGraphicsComponent(nullptr)
{
	SetName("Cues");

	SetTag("Cues");

	SetLayer("Cues");

	ResourceManager::Instance().GetResource(&pMesh,"Cues", "Resource/Cues.fbx");

	pPhysicsComponent = NEW CuesPhysics(this);
	
	pInputComponent = NEW CuesInput(this);

	pGraphicsComponent = NEW CuesGraphics(this, pMesh);

	pCollider = NEW SphereCollider();
	pCollider->Create(this, Sphere, 1.5f);

	SetWorld();
}

Cues::~Cues()
{
	pCollider->Destroy();
	SAFE_DELETE(pInputComponent);
	SAFE_DELETE(pPhysicsComponent);
	SAFE_DELETE(pGraphicsComponent);
}

void Cues::Update()
{
	pInputComponent->Update();

	pPhysicsComponent->Update();

	pGraphicsComponent->Update();

	if(InputDeviceManager::Instance().GetMouseButtonDown(0) == true)
		pCollider->Update();
}