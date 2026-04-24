#ifndef __SCENE_ROOT_H__
#define __SCENE_ROOT_H__

#include "SceneBase.hpp"
#include <DirectXMath.h>

class SceneRoot : public SceneBase
{
public:
	SceneRoot() : SceneBase("Root") {}
	void Init();
	void Uninit();
	void Update(float tick);
	void Draw();

private:
	void ChangeScene();

private:
	int m_index;
#ifdef _DEBUG
	DirectX::XMFLOAT3 m_camPos;
	DirectX::XMFLOAT3 m_camLook;
	DirectX::XMFLOAT3 m_camUp;
#endif
};

#endif // __SCENE_ROOT_H__