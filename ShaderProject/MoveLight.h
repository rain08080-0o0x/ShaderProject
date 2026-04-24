#ifndef __MOVE_LIGHT_H__
#define __MOVE_LIGHT_H__

#include "GameObject.hpp"
#include "Light.h"

class MoveLight : public GameObject
{
public:
	MoveLight();
	~MoveLight();

	void Update() final;

private:
	Light* m_pComponent;
};

#endif // __MOVE_LIGHT_H__
