#include "../Header/BilliardsTable.h"
#include "../Header/FBXRendererDX11.h"
#include "../Header/CollisionFromFBX.h"
#include "../Header/Camera.h"
#include "../Header/ResourceManager.h"
#include "../Header/Collider.h"
#include "../Header/BilliardsTablePhysics.h"
#include "../Header/BilliardsTableGraphics.h"

BilliardsTable::BilliardsTable() : GameObject()
{
	pTableModel = NEW FBXRenderDX11();
	pCollisionModel = NEW FBXRenderDX11();

	ResourceManager::Instance().GetResource(*pTableModel, "Table", "Resource/billiardsTable_light.fbx");
	ResourceManager::Instance().GetResource(*pCollisionModel, "col", "Resource/billiardsTableCollider.fbx");

	pPhysicsComponent = NEW BilliardsTablePhysics(this);

	pGraphicsComponent = NEW BilliardsTableGraphics(this);
	
	pCollider = NEW MeshCollider();
	pCollider->Create(this,Mesh,"Resource/billiardsTableCollider.fbx" ,15.0f);

	XMStoreFloat4x4(&m_World, XMMatrixIdentity());

	ang = 0.0f;
}

BilliardsTable::~BilliardsTable()
{
	pCollider->Destroy();
	SAFE_DELETE(pTableModel);
	SAFE_DELETE(pCollisionModel);
	SAFE_DELETE(pPhysicsComponent);
	SAFE_DELETE(pGraphicsComponent);
}

void BilliardsTable::Update()
{
	ang = 0.01f;

	XMStoreFloat4x4(&m_World, XMLoadFloat4x4(&m_World)*XMMatrixRotationY(ang));

	pTableModel->SetMatrix(m_World, Camera::Instance().view, Camera::Instance().proj);

	pCollisionModel->SetMatrix(m_World, Camera::Instance().view, Camera::Instance().proj);

	pPhysicsComponent->Update();

	pCollider->Update();

	pGraphicsComponent->Update();
}