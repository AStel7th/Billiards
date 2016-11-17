#pragma once
#include "GameObject.h"

class FBXRenderDX11;
class CollisionFromFBX;
class BilliardsTablePhysics;
class MeshCollider;
class BilliardsTableGraphics;

class BilliardsTable : public GameObject
{
private:
	FBXRenderDX11*				pTableModel;
	FBXRenderDX11*				pCollisionModel;
	MeshCollider*				pCollider;
	BilliardsTablePhysics*		pPhysicsComponent;
	BilliardsTableGraphics*		pGraphicsComponent;
	XMFLOAT4X4					m_World;
	float						ang;
public:
	BilliardsTable();

	~BilliardsTable();

	void Update();
};