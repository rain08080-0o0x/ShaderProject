#pragma once
#include "SceneBase.hpp"

class SceneLighting : public SceneBase
{
public:
	SceneLighting() : SceneBase("Lighting") {}

	void Init()override;
	void Uninit() override;
	void Update(float tick) override;
	void Draw() override;
};
