#pragma once
#include "GameObject.h"
#include "Component.h"
#include "GameState.h"

class MainCameraInput;

class MainCamera : public GameObject
{
private:
	MainCameraInput* pInputComponent;
public:
	MainCamera();
	virtual ~MainCamera();

	void Update();
};

class MainCameraInput : public InputComponent
{
private:
	enum CameraState
	{
		Title,
		TurnChange,
		Shot,
		FollowMovement,
		BallSet,
	};

	CameraState camState;
	GameObject* whiteBall;
	GameObject* cues;

	float moveSpeed;
	XMFLOAT3 startPos;
	XMFLOAT3 startAt;
	XMFLOAT3 followStatePos;

	void MovePosition(XMFLOAT3& nPos, XMFLOAT3& goal, XMFLOAT3& outPos);

	void GamePhase(GAME_STATE state);
public:
	MainCameraInput(GameObject* pObj);

	virtual ~MainCameraInput();

	void Update();

};