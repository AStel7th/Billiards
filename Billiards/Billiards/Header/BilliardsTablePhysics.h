#pragma once

#include "Component.h"

class GameObject;

class BilliardsTablePhysics : public PhysicsComponent
{
public:
	BilliardsTablePhysics(GameObject* pObj);
	~BilliardsTablePhysics();

	void Update();

	void receive(int message);
};