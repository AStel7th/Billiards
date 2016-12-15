#include "../Header/BilliardsTableComponents.h"
#include "../Header/GameObject.h"
#include "../Header/DrawSystem.h"
#include "../Header/MeshData.h"

BilliardsTablePhysics::BilliardsTablePhysics(GameObject* pObj) : PhysicsComponent()
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);
}

BilliardsTablePhysics::~BilliardsTablePhysics()
{

}

bool BilliardsTablePhysics::Update()
{
	return true;
}


BilliardsTableGraphics::BilliardsTableGraphics(GameObject * pObj, MeshData* mesh)
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	pMeshData = mesh;
}

BilliardsTableGraphics::~BilliardsTableGraphics()
{
}

bool BilliardsTableGraphics::Update()
{
	DrawSystem::Instance().AddDrawList(DRAW_PRIOLITY::Opaque, pMeshData->GetName(), this);

	return true;
}