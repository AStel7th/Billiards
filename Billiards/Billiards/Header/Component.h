#pragma once
#include <DirectXMath.h>
#include <vector>
#include <typeindex>
#include "func.h"

using namespace std;
using namespace DirectX;

class GameObject;
class MeshData;

class Component
{
public:
	vector<type_index> id;
	Component(){}
	virtual ~Component() {}
};

class InputComponent : public Component
{
public:
	GameObject* pGameObject;

	InputComponent() : Component()
	{
		id.push_back(typeid(this));
	}

	virtual ~InputComponent() {}
	virtual bool Update() = 0;
};

class PhysicsComponent : public Component
{
public:
	GameObject* pGameObject;
	XMFLOAT3 prePos;		// 前フレームの位置
	XMFLOAT3 velocity;		// 移動ベクトル
	XMFLOAT3 acceleration;	// 加速度
	float	 mass;			// 質量

	PhysicsComponent(): Component(), pGameObject(nullptr),prePos(0.0f,0.0f,0.0f),velocity(0.0f, 0.0f, 0.0f),acceleration(0.0f, 0.0f, 0.0f),mass(1.0f)
	{
		id.push_back(typeid(this));
	}

	virtual ~PhysicsComponent(){}
	virtual bool Update() = 0;
	virtual void OnCollisionEnter(GameObject* other) {}
};

class GraphicsComponent : public Component
{
public:
	GameObject* pGameObject;
	MeshData* pMeshData;
	int frame;

	GraphicsComponent() : Component(), pGameObject(nullptr), pMeshData(nullptr), frame(0)
	{
		id.push_back(typeid(this));
	}

	virtual ~GraphicsComponent(){}
	virtual bool Update() = 0;
};