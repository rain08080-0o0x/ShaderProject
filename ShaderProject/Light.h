#ifndef __LIGHT_BASE_H__
#define __LIGHT_BASE_H__

#include "Component.h"
#include <DirectXMath.h>

class Light : public Component
{
public:
	enum Type {
		Directional,	// 平行光源
		PointLight,		// 点光源
		SpotLight,		// スポットライト
	};
public:
	Light();
	~Light();

	void Execute() final;

	Type GetType();
	DirectX::XMFLOAT3 GetDirection();
	DirectX::XMFLOAT4 GetDiffuse();
	float GetRange();
	float GetSpotAngle();

	void SetType(Type type);
	void SetDirection(DirectX::XMFLOAT3 dir);
	void SetDiffuse(DirectX::XMFLOAT4 color);
	void SetRange(float range);
	void SetSpotAngle(float angle);

	void ReadWrite(DataAccessor* data) final;
#if _DEBUG
	void Debug(debug::Window* window) final;
	void Draw();
#endif

private:
	Type m_type;
	DirectX::XMFLOAT4 m_diffuse;
	float m_range;
	float m_spotAngle;
#ifdef _DEBUG
	bool m_isShow;
#endif
};


#endif // __LIGHT_BASE_H__