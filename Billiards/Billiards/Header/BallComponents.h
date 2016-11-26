#pragma once
#include "Component.h"

class BallPhysics : public PhysicsComponent
{
public:
	BallPhysics(GameObject* pObj);

	virtual ~BallPhysics();

	void Update();
};

class BallGraphics : public GraphicsComponent
{
public:
	BallGraphics(GameObject* pObj, MeshData* mesh);

	virtual ~BallGraphics();

	void Update();
};