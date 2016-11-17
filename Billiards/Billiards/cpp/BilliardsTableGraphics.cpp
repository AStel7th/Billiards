#include "../Header/BilliardsTableGraphics.h"
#include "../Header/GameObject.h"

BilliardsTableGraphics::BilliardsTableGraphics(GameObject * pObj)
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);
}

BilliardsTableGraphics::~BilliardsTableGraphics()
{
}

void BilliardsTableGraphics::Update()
{
}

void BilliardsTableGraphics::receive(int message)
{
}
