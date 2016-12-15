#pragma once
#include "Component.h"
#include "GameState.h"

class SoundPlayer;

class BallPhysics : public PhysicsComponent
{
private:
	bool isMove;
	SoundPlayer* hitSE;
	GameObject* firstHit;

	void GamePhase(GAME_STATE state);
public:
	BallPhysics(GameObject* pObj);

	virtual ~BallPhysics();

	bool Update();

	void OnCollisionEnter(GameObject* other);
};

class BallGraphics : public GraphicsComponent
{
private:
	float circumference; //‰~Žü‚Ì’·‚³
	PhysicsComponent* pPhysics;
public:
	BallGraphics(GameObject* pObj, MeshData* mesh,int num);

	virtual ~BallGraphics();

	bool Update();
};