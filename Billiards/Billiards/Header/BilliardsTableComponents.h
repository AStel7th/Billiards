#pragma once

#include "Component.h"

class GameObject;

class BilliardsTablePhysics : public PhysicsComponent
{
public:
	BilliardsTablePhysics(GameObject* pObj);
	~BilliardsTablePhysics();

	void Update();
};

class BilliardsTableGraphics : public GraphicsComponent
{
public:
	BilliardsTableGraphics(GameObject* pObj, MeshData* mesh);
	~BilliardsTableGraphics();

	void Update();
};