
#include "Component.h"

class GameObject;
class MeshData;

class BilliardsTableGraphics : public GraphicsComponent
{
public:
	BilliardsTableGraphics(GameObject* pObj,MeshData* mesh);
	~BilliardsTableGraphics();

	void Update();

	void receive(int message);
};