#pragma once
#include <memory>

class TimeControl;

// ゲームの核のクラスです
class Game
{
public:
	TimeControl* timeCtrl = nullptr;	//フレーム管理クラス
public:
	Game();
	~Game();

	bool Run();


};