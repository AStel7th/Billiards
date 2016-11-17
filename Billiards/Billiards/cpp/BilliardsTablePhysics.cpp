#include "../Header/BilliardsTablePhysics.h"
#include "../Header/GameObject.h"

BilliardsTablePhysics::BilliardsTablePhysics(GameObject* pObj) : PhysicsComponent()
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);
}

BilliardsTablePhysics::~BilliardsTablePhysics()
{

}

void BilliardsTablePhysics::Update()
{
	
}

void BilliardsTablePhysics::receive(int message)
{

}