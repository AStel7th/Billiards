#include "../Header/BilliardsTable.h"
#include "../Header/FBXRendererDX11.h"
#include "../Header/CollisionFromFBX.h"
#include "../Header/Camera.h"
#include "../Header/ResourceManager.h"
#include "../Header/BilliardsTablePhysicsComponent.h"

BilliardsTable::BilliardsTable() : GameObject(CALL_TAG::TABLE)
{
	pTableModel = NEW FBXRenderDX11();
	pCollisionModel = NEW FBXRenderDX11();

	ResourceManager::Instance().GetResource(*pTableModel, "Table", "Resource/billiardsTable_light.fbx");
	ResourceManager::Instance().GetResource(*pCollisionModel, "col", "Resource/billiardsTableCollider.fbx");

	pCollider = NEW CollisionFromFBX();
	pCollider->LoadFBX("Resource/billiardsTableCollider.fbx");

	pPhysicsComponent = NEW BilliardsTablePhysicsComponent();

	XMStoreFloat4x4(&m_World, XMMatrixIdentity());

	ang = 0.0f;
}

BilliardsTable::~BilliardsTable()
{
	SAFE_DELETE(pTableModel);
	SAFE_DELETE(pCollisionModel);
	SAFE_DELETE(pCollider);
	SAFE_DELETE(pPhysicsComponent);
}

void BilliardsTable::Update()
{
	ang = 0.01f;

	XMStoreFloat4x4(&m_World, XMLoadFloat4x4(&m_World)*XMMatrixRotationY(ang));

	pTableModel->SetMatrix(m_World, Camera::Instance().view, Camera::Instance().proj);

	pCollisionModel->SetMatrix(m_World, Camera::Instance().view, Camera::Instance().proj);

	pPhysicsComponent->Update(*this);
}

void BilliardsTable::Draw()
{
	pTableModel->Draw();
	pCollisionModel->Draw();
}