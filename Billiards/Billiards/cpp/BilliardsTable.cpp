#include "../Header/BilliardsTable.h"
#include "../Header/ResourceManager.h"
#include "../Header/Collider.h"
#include "../Header/BilliardsTableComponents.h"
#include "../Header/MeshData.h"

BilliardsTable::BilliardsTable() : GameObject()
{
	//pMesh = ResourceManager::Instance().GetResource("Table", "Resource/billiardsTableCollider.fbx");
	pMesh = ResourceManager::Instance().GetResource("Table", "Resource/BilliardsTableModel.fbx");
	//pMesh = ResourceManager::Instance().GetResource("Table", "Resource/goblin2.fbx");

	pPhysicsComponent = NEW BilliardsTablePhysics(this);

	pGraphicsComponent = NEW BilliardsTableGraphics(this,pMesh);
	
	pCollider = NEW MeshCollider();
	pCollider->Create(this,Mesh,"Resource/billiardsTableCollider.fbx" ,180.0f);

	XMMATRIX wMat = XMMatrixIdentity();
	wMat *= XMMatrixRotationQuaternion(XMVectorSet(rot.x, rot.y, rot.z, 1.0f));
	wMat *= XMMatrixScaling(scale.x, scale.y, scale.z);
	wMat *= XMMatrixTranslation(pos.x, pos.y, pos.z);

	XMStoreFloat4x4(&world, wMat);
}

BilliardsTable::~BilliardsTable()
{
	pCollider->Destroy();
	SAFE_DELETE(pPhysicsComponent);
	SAFE_DELETE(pGraphicsComponent);
}

void BilliardsTable::Update()
{
	pPhysicsComponent->Update();

	pGraphicsComponent->Update();

	XMMATRIX wMat = XMMatrixIdentity();
	wMat *= XMMatrixRotationQuaternion(XMVectorSet(rot.x, rot.y, rot.z, 1.0f));
	wMat *= XMMatrixScaling(scale.x, scale.y, scale.z);
	wMat *= XMMatrixTranslation(pos.x, pos.y, pos.z);

	XMStoreFloat4x4(&world, wMat);

	pCollider->Update();
}