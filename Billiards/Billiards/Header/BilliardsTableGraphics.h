
#include "Component.h"

class GameObject;

class BilliardsTableGraphics : public GraphicsComponent
{
public:
	BilliardsTableGraphics(GameObject* pObj);
	~BilliardsTableGraphics();

	void Update();

	void receive(int message);
};