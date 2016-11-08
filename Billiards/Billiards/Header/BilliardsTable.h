#pragma once
#include <memory>
#include "FBXRendererDX11.h"
#include "CollisionFromFBX.h"
#include "GameObjectManager.h"
#include "Direct3D11.h"
#include "DrawSystem.h"

class BilliardsTable : public GameObject
{
private:
	FBXRenderDX11 model;
	FBXRenderDX11 collision;

	CollisionFromFBX col;

	XMFLOAT4X4 m_World;

	float ang;
public:
	BilliardsTable();

	~BilliardsTable();

	void Update();

	void Draw();
};