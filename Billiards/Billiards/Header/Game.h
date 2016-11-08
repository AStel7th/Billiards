#ifndef __CSCENE_H__
#define __CSCENE_H__

#include "TimeControl.h"
#include "Direct3D11.h"
#include "DrawFPS.h"
#include "BilliardsTable.h"

// ゲームの核のクラスです
class Game
{
public:
	Direct3D11 &d3d11 = Direct3D11::Instance();
	unique_ptr<TimeControl> timeCtrl = nullptr;	//フレーム管理クラス
public:
	Game();
	~Game();

	bool Run();


};

#endif