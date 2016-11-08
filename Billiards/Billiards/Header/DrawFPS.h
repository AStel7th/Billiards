#pragma once
#include "../Header/GameObjectManager.h"
#include "../Header/DebugFont.h"

class DrawFPS : public GameObject
{
private:
	unique_ptr<UFPS> pFPS;
public:
	DrawFPS();
	~DrawFPS();

	void Update();

	void Draw();
};