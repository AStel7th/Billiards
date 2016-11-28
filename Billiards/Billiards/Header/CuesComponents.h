#pragma once
#include "Component.h"

class CuesInput : public InputComponent
{
private:
	GameObject* whiteBall;
public:
	CuesInput(GameObject* pObj);

	virtual ~CuesInput();

	void Update();
};

class CuesPhysics : public PhysicsComponent
{
public:
	CuesPhysics(GameObject* pObj);

	virtual ~CuesPhysics();

	void Update();
};

class CuesGraphics : public GraphicsComponent
{
public:
	CuesGraphics(GameObject* pObj, MeshData* mesh);

	virtual ~CuesGraphics();

	void Update();
};
