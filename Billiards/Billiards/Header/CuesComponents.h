#pragma once
#include "Component.h"

class CuesPhysics;

class CuesControllerInput : public InputComponent
{
private:
	GameObject* whiteBall;
public:
	CuesControllerInput(GameObject* pObj);

	virtual ~CuesControllerInput();

	void Update();
};

class CuesInput : public InputComponent
{
private:
	CuesPhysics* pPhysics;
	float movePos;
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

	void OnCollisionEnter(GameObject* other);
};

class CuesGraphics : public GraphicsComponent
{
public:
	CuesGraphics(GameObject* pObj, MeshData* mesh);

	virtual ~CuesGraphics();

	void Update();
};
