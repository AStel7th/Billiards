#pragma once
#include <memory>

class TimeControl;

// �Q�[���̊j�̃N���X�ł�
class Game
{
public:
	TimeControl* timeCtrl = nullptr;	//�t���[���Ǘ��N���X
public:
	Game();
	~Game();

	bool Run();


};