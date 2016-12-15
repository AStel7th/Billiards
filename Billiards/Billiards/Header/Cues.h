#pragma once
#include "GameObject.h"
#include "GameState.h"

class CuesControllerInput;
class MeshData;
class SphereCollider;
class CuesInput;
class CuesPhysics;
class CuesGraphics;

class CuesController : public GameObject
{
private:
	CuesControllerInput*			pInputComponent;

	void ShotPhase(GAME_STATE state);
public:
	CuesController();
	virtual ~CuesController();

	void Update();
};

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