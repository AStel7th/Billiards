#include "../Header/Ball.h"
#include "../Header/ResourceManager.h"
#include "../Header/Collider.h"
#include "../Header/BallComponents.h"

Ball::Ball(int num,float x, float y, float z) : GameObject(), pMesh(nullptr), pCollider(nullptr), pPhysicsComponent(nullptr), pGraphicsComponent(nullptr)
{
	SetName("Ball" + to_string(num));

	SetTag("Ball");

	SetLayer("Ball");

	ResourceManager::Instance().GetResource(&pMesh, "Ball" + to_string(num), "Resource/Ball.fbx");

	pPhysicsComponent = NEW BallPhysics(this);

	pGraphicsComponent = NEW BallGraphics(this, pMesh,num);

	pCollider = NEW SphereCollider();
	pCollider->Create(this, Sphere, 2.6f);

	pos.x = x;
	pos.y = y;
	pos.z = z;

	SetWorld();
}

Ball::~Ball()
{
	pCollider->Destroy();
	SAFE_DELETE(pPhysicsComponent);
	SAFE_DELETE(pGraphicsComponent);
}

void Ball::Update()
{
	pPhysicsComponent->Update();

	pGraphicsComponent->Update();
	
	pCollider->Update();
}