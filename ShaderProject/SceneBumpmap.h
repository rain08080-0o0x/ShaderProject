#ifndef __SCENE_BUMPMAP_H__
#define __SCENE_BUMPMAP_H__

#include "SceneBase.hpp"

class SceneBumpmap : public SceneBase
{
public:
	SceneBumpmap() : SceneBase("Bumpmap") {}
	void Init();
	void Uninit();
	void Update(float tick);
	void Draw();
};

#endif // __SCENE_BUMPMAP_H__
