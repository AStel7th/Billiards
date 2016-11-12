#pragma once

#include "Component.h"

class GameObject;

class BilliardsTablePhysicsComponent : public PhysicsComponent
{
public:
	BilliardsTablePhysicsComponent();
	~BilliardsTablePhysicsComponent();

	void Update(GameObject& object);

	void receive(int message);
};