#pragma once
#include "GameObject.h"
#include "Component.h"

class MeshData;
class HouseGraphics;

class House : public GameObject
{
private:
	MeshData*					pMesh;
	HouseGraphics*		pGraphicsComponent;
public:
	House();
	virtual ~House();

	void Update();
};

class HouseGraphics : public GraphicsComponent
{
public:
	HouseGraphics(GameObject * pObj, MeshData* mesh);
	virtual ~HouseGraphics();

	void Update();
};