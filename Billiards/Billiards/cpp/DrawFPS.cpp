#include "../Header/DrawFPS.h"
#include "../Header/DebugFont.h"

DrawFPS::DrawFPS() : GameObject(CALL_TAG::BALL)
{
	pFPS = NEW UFPS();
	pFPS->CreateMesh();
}

DrawFPS::~DrawFPS()
{
	SAFE_DELETE(pFPS);
}

void DrawFPS::Update()
{
	pFPS->Update();
}

void DrawFPS::Draw()
{
	pFPS->Draw();
}