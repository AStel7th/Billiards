#include "../Header/Pocket.h"
#include "../Header/Collider.h"
#include <string>
#include <tchar.h>
#include "../Header/Messenger.h"

Pocket::Pocket(int id,float x, float y, float z) : GameObject(),pCollider(nullptr), pPhysicsComponent(nullptr)
{
	SetName("Pocket" + to_string(id));

	SetTag("Pocket");

	SetLayer("Pocket");

	pPhysicsComponent = NEW PocketPhysics(this);

	pCollider = NEW BoxCollider();
	pCollider->Create(this, Box, 30.0f);

	pos.x = x;
	pos.y = y;
	pos.z = z;
}

Pocket::~Pocket()
{
	pCollider->Destroy();
	SAFE_DELETE(pPhysicsComponent);
}

void Pocket::Update()
{
	pPhysicsComponent->Update();

	pCollider->Update();
}




PocketPhysics::PocketPhysics(GameObject* pObj) : PhysicsComponent()
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	prePos = pGameObject->pos;
}

PocketPhysics::~PocketPhysics()
{
}

void PocketPhysics::Update()
{
}

void PocketPhysics::OnCollisionEnter(GameObject * other)
{
	if (other->tag == "Ball")
		Messenger::SetBallInPocket(other);

	if(other->name != "HandBall")
		other->SetActive(false);
}