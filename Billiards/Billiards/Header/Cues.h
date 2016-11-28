#pragma once
#include "GameObject.h"

class MeshData;
class SphereCollider;
class CuesInput;
class CuesPhysics;
class CuesGraphics;

class Cues : public GameObject
{
private:
	MeshData*					pMesh;
	SphereCollider*				pCollider;
	CuesInput*			pInputComponent;
	CuesPhysics*		pPhysicsComponent;
	CuesGraphics*		pGraphicsComponent;
public:
	Cues();
	virtual ~Cues();

	void Update();
};