#pragma once
#include "GameObject.h"
#include "Component.h"

class BoxCollider;
class PocketPhysics;

class Pocket : public GameObject
{
private:
	BoxCollider*	pCollider;
	PocketPhysics*  pPhysicsComponent;
public:
	Pocket(int id,float x,float y,float z);

	virtual ~Pocket();

	void Update();
};

class PocketPhysics : public PhysicsComponent
{
public:
	PocketPhysics(GameObject* pObj);

	virtual ~PocketPhysics();

	bool Update();

	void OnCollisionEnter(GameObject* other);
};