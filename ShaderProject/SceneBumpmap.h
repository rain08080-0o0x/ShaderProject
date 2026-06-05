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

private:
	float m_metallic = 0.0f;
	float m_roughness = 0.5f;
	float m_specular = 0.5f;
	float m_baseColorPower = 1.0f;
	float m_heightScale = 0.03f;
};

#endif // __SCENE_BUMPMAP_H__
