#include "../Header/BilliardsTable.h"
#include "../Header/FBXRendererDX11.h"
#include "../Header/CollisionFromFBX.h"
#include "../Header/Camera.h"
#include "../Header/ResourceManager.h"

BilliardsTable::BilliardsTable() : GameObject(CALL_TAG::TABLE)
{
	model = NEW FBXRenderDX11();
	collision = NEW FBXRenderDX11();

	ResourceManager::Instance().GetResource(*model, "Table", "Resource/billiardsTable_light.fbx");
	ResourceManager::Instance().GetResource(*collision, "col", "Resource/billiardsTableCollider.fbx");

	col = NEW CollisionFromFBX();
	col->LoadFBX("Resource/billiardsTableCollider.fbx");

	XMStoreFloat4x4(&m_World, XMMatrixIdentity());

	ang = 0.0f;
}

BilliardsTable::~BilliardsTable()
{
	SAFE_DELETE(model);
	SAFE_DELETE(collision);
	SAFE_DELETE(col);
}

void BilliardsTable::Update()
{
	ang = 0.01f;

	XMStoreFloat4x4(&m_World, XMLoadFloat4x4(&m_World)*XMMatrixRotationY(ang));

	model->SetMatrix(m_World, Camera::Instance().view, Camera::Instance().proj);

	collision->SetMatrix(m_World, Camera::Instance().view, Camera::Instance().proj);
}

void BilliardsTable::Draw()
{
	model->Draw();
	collision->Draw();
}