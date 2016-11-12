#ifndef __BILLIARDSTABLE_H__
#define __BILLIARDSTABLE_H__

#include "GameObject.h"

class FBXRenderDX11;
class CollisionFromFBX;
class GameObject;

class BilliardsTable : public GameObject
{
private:
	FBXRenderDX11* model;
	FBXRenderDX11* collision;

	CollisionFromFBX* col;

	XMFLOAT4X4 m_World;

	float ang;
public:
	BilliardsTable();

	~BilliardsTable();

	void Update();

	void Draw();
};

#endif