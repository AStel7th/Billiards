#include "../Header/House.h"
#include "../Header/MeshData.h"
#include "../Header/ResourceManager.h"
#include "../Header/DrawSystem.h"

House::House() : GameObject(),pMesh(nullptr),pGraphicsComponent(nullptr)
{
	SetName("House");

	SetTag("House");

	ResourceManager::Instance().GetResource(&pMesh,"House","Resource/BilliardsHouse.fbx");

	pGraphicsComponent = NEW HouseGraphics(this, pMesh);

	pos.x = -450.0f;
	pos.z = -550.0f;

	SetWorld();
}

House::~House()
{
	SAFE_DELETE(pGraphicsComponent);
}

void House::Update()
{
	pGraphicsComponent->Update();
}


HouseGraphics::HouseGraphics(GameObject * pObj, MeshData* mesh) : GraphicsComponent()
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	pMeshData = mesh;
}

HouseGraphics::~HouseGraphics()
{
}

bool HouseGraphics::Update()
{
	DrawSystem::Instance().AddDrawList(DRAW_PRIOLITY::Opaque, pMeshData->GetName(), this);

	return true;
}
