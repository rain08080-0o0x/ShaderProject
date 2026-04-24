#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Component.h"
#include <DirectXMath.h>

class Camera : public Component
{
public:
	Camera();
	virtual ~Camera();

	void ReadWrite(DataAccessor* data) final;
#if _DEBUG
	void Debug(debug::Window* window) final;
	void Draw();
#endif

	DirectX::XMFLOAT4X4 GetView(bool transpose = true);
	DirectX::XMFLOAT4X4 GetProj(bool transpose = true);

	bool Is3D();
	float GetFovy();
	float GetWidth();
	float GetNear();
	float GetFar();
	float GetAspect();
	float GetFocus();
	DirectX::XMFLOAT3 GetLook();

	void SetFocus(float focus);

private:
	bool m_is3D;
	float m_fovy;
	float m_width;
	float m_aspect, m_near, m_far;
	float m_focus;
#ifdef _DEBUG
	bool m_isShow;
#endif
};

#endif // __CAMERA_H__