#ifndef __SCENE_TOON_H__
#define __SCENE_TOON_H__

#include "SceneBase.hpp"

class SceneToon : public SceneBase
{
public:
	SceneToon() : SceneBase("Toon") {}
	void Init();
	void Uninit();
	void Update(float tick);
	void Draw();

private:
	float m_alphaDitherAlpha = 1.0f;
	bool m_alphaDitherUseDistanceFade = true;
	float m_alphaDitherFadeStart = 3.0f;
	float m_alphaDitherFadeEnd = 8.0f;
};

#endif // __SCENE_TOON_H__
