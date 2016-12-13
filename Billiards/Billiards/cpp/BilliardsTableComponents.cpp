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

void BilliardsTablePhysics::Update()
{
	
}


BilliardsTableGraphics::BilliardsTableGraphics(GameObject * pObj, MeshData* mesh)
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	pMeshData = mesh;

	/*DrawSystem::Instance().AddDrawList(DRAW_PRIOLITY::Opaque, pMeshData->GetName(), this);*/
}

BilliardsTableGraphics::~BilliardsTableGraphics()
{
}

void BilliardsTableGraphics::Update()
{
	//frame++;
	DrawSystem::Instance().AddDrawList(DRAW_PRIOLITY::Opaque, pGameObject->name, this);
}