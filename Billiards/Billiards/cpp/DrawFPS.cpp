#include "../Header/DrawFPS.h"

DrawFPS::DrawFPS() : GameObject(CALL_TAG::BALL)
{
	pFPS = make_unique<UFPS>();
	pFPS->CreateMesh();
}

DrawFPS::~DrawFPS()
{

}

void DrawFPS::Update()
{
	pFPS->Update();
}

void DrawFPS::Draw()
{
	pFPS->Draw();
}