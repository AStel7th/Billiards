#pragma once
#include "Component.h"

class SoundPlayer;

class BallPhysics : public PhysicsComponent
{
private:
	bool isMove;
	SoundPlayer* hitSE;
public:
	BallPhysics(GameObject* pObj);

	virtual ~BallPhysics();

	void Update();

	void OnCollisionEnter(GameObject* other);
};

class BallGraphics : public GraphicsComponent
{
public:
	BallGraphics(GameObject* pObj, MeshData* mesh,int num);

	virtual ~BallGraphics();

	void Update();
};