#ifndef __DRAWFPS_H__
#define __DRAWFPS_H__

#include "GameObject.h"

class UFPS;

class DrawFPS : public GameObject
{
private:
	UFPS* pFPS;
public:
	DrawFPS();
	~DrawFPS();

	void Update();

	void Draw();
};

#endif