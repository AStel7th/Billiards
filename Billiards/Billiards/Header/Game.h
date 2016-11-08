#ifndef __CSCENE_H__
#define __CSCENE_H__

#include "TimeControl.h"
#include "Direct3D11.h"
#include "DrawFPS.h"
#include "BilliardsTable.h"

// �Q�[���̊j�̃N���X�ł�
class Game
{
public:
	Direct3D11 &d3d11 = Direct3D11::Instance();
	unique_ptr<TimeControl> timeCtrl = nullptr;	//�t���[���Ǘ��N���X
public:
	Game();
	~Game();

	bool Run();


};

#endif