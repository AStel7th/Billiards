#pragma once
#include "GameObject.h"

class FBXRenderDX11;
class CollisionFromFBX;
class BilliardsTablePhysics;
class MeshCollider;
class BilliardsTableGraphics;
class MeshData;

class BilliardsTable : public GameObject
{
private:
	MeshData*					pMesh;
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