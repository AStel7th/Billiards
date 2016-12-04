#include "../Header/CuesComponents.h"
#include "../Header/GameObject.h"
#include "../Header/DrawSystem.h"
#include "../Header/MeshData.h"
#include "../Header/InputDeviceManager.h"

CuesControllerInput::CuesControllerInput(GameObject * pObj) : InputComponent(), whiteBall(nullptr)
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	whiteBall = GameObject::All::GameObjectFindWithName("Ball0");

	pGameObject->pos = whiteBall->pos;
}

CuesControllerInput::~CuesControllerInput()
{
}

void CuesControllerInput::Update()
{
	pGameObject->pos = whiteBall->pos;

	if(!InputDeviceManager::Instance().GetMouseButtonDown(0))
		pGameObject->rot.y -= RADIAN(InputDeviceManager::Instance().GetMouseState().x) / 10.0f;

	pGameObject->SetWorld();
}

CuesInput::CuesInput(GameObject * pObj) : InputComponent(), movePos(10.0f), pPhysics(nullptr)
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	pPhysics = GetComponent<CuesPhysics>(pGameObject);

	
	/*XMVECTOR ballPos = XMLoadFloat3(&whiteBall->pos);
	XMVECTOR behindVec = XMVectorSet(whiteBall->world._31, whiteBall->world._32, whiteBall->world._33, whiteBall->world._34);
	behindVec = XMVector3Normalize(behindVec);
	XMVECTOR _pos = ballPos - behindVec * 10.0f;
	XMStoreFloat3(&pGameObject->pos, _pos);
	XMStoreFloat3(&pPhysics->prePos, _pos);*/

	pGameObject->pos.x = 10.0f;
	pGameObject->rot.x = RADIAN(-85.0f);
	pGameObject->rot.y = RADIAN(-90.0f);
}

CuesInput::~CuesInput()
{
}

void CuesInput::Update()
{
	XMFLOAT3 worldPrePos = pGameObject->GetWorldPos();
	pPhysics->prePos = pGameObject->pos;

	/*XMVECTOR ballPos = XMLoadFloat3(&whiteBall->pos);
	XMVECTOR behindVec = XMVectorSet(whiteBall->world._11, whiteBall->world._12, whiteBall->world._13, whiteBall->world._14);
	behindVec = XMVector3Normalize(behindVec);*/
	pGameObject->pos.x += InputDeviceManager::Instance().GetMouseState().y / 10.0f;
	/*XMStoreFloat3(&pGameObject->pos, _pos);*/

	pGameObject->SetWorld();

	pPhysics->velocity.x = pGameObject->GetWorldPos().x - worldPrePos.x;
	pPhysics->velocity.y = pGameObject->GetWorldPos().y - worldPrePos.y;
	pPhysics->velocity.z = pGameObject->GetWorldPos().z - worldPrePos.z;

	TCHAR s[256];

	_stprintf_s(s, _T("pos:%f %f %f \n"), pPhysics->velocity.x, pPhysics->velocity.y, pPhysics->velocity.z);

	OutputDebugString(s);
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

void CuesPhysics::Update()
{
	
}

void CuesPhysics::OnCollisionEnter(GameObject* other)
{
	if(other->tag != "Table")
		pGameObject->pParent->SetActive(false);
	//pGameObject->Destroy();
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

void CuesGraphics::Update()
{
	DrawSystem::Instance().AddDrawList(DRAW_PRIOLITY::Opaque, pMeshData->GetName(), this);
}
