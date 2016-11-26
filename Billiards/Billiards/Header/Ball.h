#pragma once
#include "GameObject.h"

class MeshData;
class SphereCollider;
class BallPhysics;
class BallGraphics;

class Ball : public GameObject
{
private:
	MeshData*					pMesh;
	SphereCollider*				pCollider;
	BallPhysics*		pPhysicsComponent;
	BallGraphics*		pGraphicsComponent;
public:
	Ball(int num, float x, float y, float z);

	~Ball();

	void Update();
};
