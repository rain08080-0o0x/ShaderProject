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
#ifdef _DEBUG
	void Debug(debug::Window* window) final;
#endif

private:
	Light* m_pComponent;
	float m_moveSpeed;
};

#endif // __MOVE_LIGHT_H__
