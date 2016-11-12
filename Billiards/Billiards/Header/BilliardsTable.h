#pragma once
#include "GameObject.h"

class FBXRenderDX11;
class CollisionFromFBX;
class BilliardsTablePhysicsComponent;

class BilliardsTable : public GameObject
{
private:
	FBXRenderDX11* pTableModel;
	FBXRenderDX11* pCollisionModel;

	CollisionFromFBX* pCollider;

	BilliardsTablePhysicsComponent* pPhysicsComponent;

	XMFLOAT4X4 m_World;

	float ang;
public:
	BilliardsTable();

	~BilliardsTable();

	void Update();

	void Draw();
};