#pragma once
#include "GameObject.h"

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
public:
	BilliardsTable();

	~BilliardsTable();

	void Update();
};