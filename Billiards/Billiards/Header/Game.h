#pragma once
#include <memory>

class TimeControl;

// ゲームの核のクラスです
class Game
{
private:
	static bool isExit;
public:
	TimeControl* timeCtrl = nullptr;	//フレーム管理クラス
public:
	Game();
	~Game();

	bool Run();

	static void ExitGame()
	{
		isExit = true;
	}
};