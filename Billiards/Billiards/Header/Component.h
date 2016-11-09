#ifndef __COMPONENT_H__
#define __COMPONENT_H__

class Component
{
public:
	virtual ~Component() {}
	virtual void receive(int message) = 0;
};

#endif