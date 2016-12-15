#include "../Header/CuesComponents.h"
#include "../Header/GameObject.h"
#include "../Header/DrawSystem.h"
#include "../Header/MeshData.h"
#include "../Header/InputDeviceManager.h"
#include "../Header/Messenger.h"

CuesControllerInput::CuesControllerInput(GameObject * pObj) : InputComponent(), whiteBall(nullptr)
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	whiteBall = GameObject::All::GameObjectFindWithName("HandBall");
}

CuesControllerInput::~CuesControllerInput()
{
}

bool CuesControllerInput::Update()
{
	pGameObject->pos = whiteBall->pos;

	if(!InputDeviceManager::Instance().GetMouseButtonDown(0))
		pGameObject->rot.y -= RADIAN(InputDeviceManager::Instance().GetMouseState().x) / 10.0f;

	pGameObject->SetWorld();

	return true;
}

CuesInput::CuesInput(GameObject * pObj) : InputComponent(), movePos(10.0f), pPhysics(nullptr)
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	pPhysics = GetComponent<CuesPhysics>(pGameObject);

	pGameObject->pos.x = 30.0f;
	pGameObject->rot.x = RADIAN(-85.0f);
	pGameObject->rot.y = RADIAN(-90.0f);

	Messenger::OnGamePhase.Add(*this, &CuesInput::ShotPhase);
}

CuesInput::~CuesInput()
{
	Messenger::OnGamePhase.Remove(*this, &CuesInput::ShotPhase);
}

bool CuesInput::Update()
{
	XMFLOAT3 worldPrePos = pGameObject->GetWorldPos();
	pPhysics->prePos = pGameObject->pos;

	pGameObject->pos.x += InputDeviceManager::Instance().GetMouseState().y / 10.0f;

	pGameObject->SetWorld();

	pPhysics->velocity.x = pGameObject->GetWorldPos().x - worldPrePos.x;
	pPhysics->velocity.y = pGameObject->GetWorldPos().y - worldPrePos.y;
	pPhysics->velocity.z = pGameObject->GetWorldPos().z - worldPrePos.z;

	return true;
}

void CuesInput::ShotPhase(GAME_STATE state)
{
	if (state == GAME_STATE::Shot)
	{
		pGameObject->pos.x = 30.0f;
		pGameObject->rot.x = RADIAN(-85.0f);
		pGameObject->rot.y = RADIAN(-90.0f);

		pGameObject->SetWorld();
	}
}


CuesPhysics::CuesPhysics(GameObject * pObj) : PhysicsComponent()
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	mass = 550.0f;  // ƒLƒ…[‚ª‘å‘Ì450g`650g‚Æ‚Ì‚±‚Æ‚È‚Ì‚ÅAŠÔ‚ðŽæ‚Á‚Ä550g
}

CuesPhysics::~CuesPhysics()
{
}

bool CuesPhysics::Update()
{
	return true;
}

void CuesPhysics::OnCollisionEnter(GameObject* other)
{
	if(other->tag != "Table")
		pGameObject->pParent->SetActive(false);
}

CuesGraphics::CuesGraphics(GameObject * pObj, MeshData * mesh) : GraphicsComponent()
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	pMeshData = mesh;
}

CuesGraphics::~CuesGraphics()
{
}

bool CuesGraphics::Update()
{
	DrawSystem::Instance().AddDrawList(DRAW_PRIOLITY::Opaque, pMeshData->GetName(), this);

	return true;
}
