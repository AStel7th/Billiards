#ifndef __CSCENE_H__
#define __CSCENE_H__

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

#endif