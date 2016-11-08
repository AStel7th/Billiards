#include "../Header/BilliardsTable.h"
#include "../Header/Camera.h"
#include "../Header/ResourceManager.h"

BilliardsTable::BilliardsTable() : GameObject(CALL_TAG::TABLE)
{
	ResourceManager::Instance().GetResource(model, "Table", "Resource/billiardsTable_light.fbx");
	ResourceManager::Instance().GetResource(collision, "col", "Resource/billiardsTableCollider.fbx");

	col.LoadFBX("Resource/billiardsTableCollider.fbx");

	XMStoreFloat4x4(&m_World, XMMatrixIdentity());

	ang = 0.0f;
}

BilliardsTable::~BilliardsTable()
{
	//SAFE_DELETE(g_pFbxDX11);
}

void BilliardsTable::Update()
{
	ang = 0.01f;

	XMStoreFloat4x4(&m_World, XMLoadFloat4x4(&m_World)*XMMatrixRotationY(ang));

	model.SetMatrix(m_World, Camera::Instance().view, Camera::Instance().proj);

	collision.SetMatrix(m_World, Camera::Instance().view, Camera::Instance().proj);
}

void BilliardsTable::Draw()
{
	model.Draw();
	collision.Draw();
}