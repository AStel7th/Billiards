#include "../Header/BilliardsTableGraphics.h"
#include "../Header/GameObject.h"
#include "../Header/DrawSystem.h"
#include "../Header/MeshData.h"

BilliardsTableGraphics::BilliardsTableGraphics(GameObject * pObj, MeshData* mesh)
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	pMeshData = mesh;

	XMMATRIX _world = XMMatrixIdentity();
	_world = XMMatrixRotationQuaternion(XMVectorSet(pGameObject->rot.x, pGameObject->rot.y, pGameObject->rot.z, 1.0f));
	_world = XMMatrixTranslation(pGameObject->pos.x, pGameObject->pos.y, pGameObject->pos.z);

	XMStoreFloat4x4(&world, _world);

	DrawSystem::Instance().AddDrawList(DRAW_PRIOLITY::Opaque, pMeshData->GetName(), this);
}

BilliardsTableGraphics::~BilliardsTableGraphics()
{
}

void BilliardsTableGraphics::Update()
{
	//frame++;
}

void BilliardsTableGraphics::receive(int message)
{
}
