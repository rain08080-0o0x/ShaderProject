#ifndef __SCENE_SHADING_H__
#define __SCENE_SHADING_H__

#include "SceneBase.hpp"

class SceneShading : public SceneBase
{
public:
	SceneShading() : SceneBase("Shading") {}
	void Init();
	void Uninit();
	void Update(float tick);
	void Draw();
};

#endif // __SCENE_SHADING_H__