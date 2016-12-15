#pragma once
#include "GameObject.h"
#include "Component.h"
#include "GameState.h"

class MeshData;
class SphereCollider;
class HandBallInput;
class BallPhysics;
class BallGraphics;

class HandBall : public GameObject
{
private:
	MeshData*					pMesh;
	SphereCollider*				pCollider;
	HandBallInput*		pInputComponent;
	BallPhysics*		pPhysicsComponent;
	BallGraphics*		pGraphicsComponent;
public:
	HandBall(float x, float y, float z);

	~HandBall();

	void Update();
};

class HandBallInput : public InputComponent
{
private:
	PhysicsComponent* handBallPhysics;
	
	void BallSetDone();
public:
	bool setFlag;

	HandBallInput(GameObject* pObj);

	virtual ~HandBallInput();

	bool Update();

	void GamePhase(GAME_STATE state);

	
};