#include "../Header/GameObject.h"

GameObject::GameObject(CALL_TAG tag)
{
	GameObjectManager::AddGameObject(this, tag);
}

GameObject::~GameObject()
{

}

unordered_map<CALL_TAG, vector<GameObject*>> GameObjectManager::objectList;

void GameObjectManager::AddGameObject(GameObject* pGameObject, CALL_TAG tag)
{
	objectList[tag].push_back(pGameObject);
}

void GameObjectManager::Update(CALL_TAG tag)
{
	vector<GameObject*> list = objectList[tag];

	for each (GameObject* var in list)
	{
		var->Update();
	}
}

void GameObjectManager::Draw(CALL_TAG tag)
{
	vector<GameObject*> list = objectList[tag];

	for each (GameObject* var in list)
	{
		var->Draw();
	}
}

void GameObjectManager::AllClear()
{
	for (auto itr = objectList.begin(); itr != objectList.end(); itr++)
	{
		vector<GameObject*> list = itr->second;

		for each (GameObject* var in list)
		{
			SAFE_DELETE(var);
		}
	}

	objectList.clear();
}