#include "../Header/Cues.h"
#include "../Header/ResourceManager.h"
#include "../Header/Collider.h"
#include "../Header/CuesComponents.h"

CuesController::CuesController() : GameObject(), pInputComponent(nullptr)
{
	SetName("CuesController");

	SetTag("CuesController");

	pInputComponent = NEW CuesControllerInput(this);

	SetWorld();
	
	AddChild(Create<Cues>());
}

CuesController::~CuesController()
{
	SAFE_DELETE(pInputComponent);
}

void CuesController::Update()
{
	pInputComponent->Update();
}

Cues::Cues() : GameObject(),pMesh(nullptr),pCollider(nullptr),pInputComponent(nullptr),pPhysicsComponent(nullptr),pGraphicsComponent(nullptr)
{
	pMesh = ResourceManager::Instance().GetResource("Cues", "Resource/Cues.fbx");

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

	pCollider->Update();
}