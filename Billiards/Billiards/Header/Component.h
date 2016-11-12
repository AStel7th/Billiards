#ifndef __COMPONENT_H__
#define __COMPONENT_H__

class GameObject;

class Component
{
public:
	virtual ~Component() {}
	virtual void receive(int message) = 0;
};

class InputComponent : public Component
{
public:
	virtual ~InputComponent() {}
	virtual void Update(GameObject& object) = 0;
};

class PhysicsComponent : public Component
{
public:
	virtual ~PhysicsComponent(){}
	virtual void Update(GameObject& object) = 0;
};

class GraphicsComponent : public Component
{
public:
	virtual ~GraphicsComponent(){}
	virtual void Update(GameObject& object) = 0;
};

#endif