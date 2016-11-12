#pragma once
#include <unordered_map>
#include <vector>
#include "func.h"

using namespace std;

enum struct CALL_TAG
{
	BALL,
	TABLE
};

class GameObject
{
public:
	float posX, posY, posZ;
	float rotX, rotY, rotZ;

	GameObject(CALL_TAG tag);
	GameObject(const GameObject & gameObject) = delete;

	virtual ~GameObject();

	virtual void Update() = 0;

	virtual void Draw() = 0;
};

class GameObjectManager
{
private:
	static unordered_map<CALL_TAG,vector<GameObject*>> objectList;
public:
	static void AddGameObject(GameObject* pGameObject, CALL_TAG tag);

	static void Update(CALL_TAG tag);

	static void Draw(CALL_TAG tag);

	static void AllClear();
};

template <class TYPE, typename ... ARGS>
static TYPE* Create(ARGS && ... args)
{
	return NEW TYPE(std::forward<ARGS>(args) ...);
}