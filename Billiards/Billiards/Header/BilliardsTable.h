#pragma once
#include "GameObject.h"

class BilliardsTablePhysics;
class MeshCollider;
class BilliardsTableGraphics;
class MeshData;

const int POCKET_COUNT = 6;

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