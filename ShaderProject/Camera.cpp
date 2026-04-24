#include "Camera.h"
#include "Geometory.h"
#include "GameObject.hpp"
#include "Geometory.h"

Camera::Camera()
	: m_is3D(true)
	, m_fovy(60.0f), m_width(20.0f)
	, m_aspect(16.0f / 9.0f), m_near(0.2f), m_far(1000.0f)
	, m_focus(1.0f)
#ifdef _DEBUG
	, m_isShow(false)
#endif
{
}
Camera::~Camera()
{
}

void Camera::ReadWrite(DataAccessor* data)
{
	data->Access<bool>(&m_is3D);
	data->Access<float>(&m_fovy);
	data->Access<float>(&m_width);
	data->Access<float>(&m_near);
	data->Access<float>(&m_far);
	data->Access<float>(&m_aspect);
	data->Access<float>(&m_focus);
#ifdef _DEBUG
	data->Access<bool>(&m_isShow);
#endif
}
#ifdef _DEBUG
void Camera::Debug(debug::Window* window)
{
	debug::Item* group = debug::Item::CreateGroup("Camera Component");
	group->AddGroupItem(debug::Item::CreateBind("is3D",		debug::Item::Bool,	&m_is3D));
	group->AddGroupItem(debug::Item::CreateBind("Fovy",		debug::Item::Float, &m_fovy));
	group->AddGroupItem(debug::Item::CreateBind("Focus",	debug::Item::Float, &m_focus));
	group->AddGroupItem(debug::Item::CreateBind("Width",	debug::Item::Float, &m_width));
	group->AddGroupItem(debug::Item::CreateBind("Near",		debug::Item::Float, &m_near));
	group->AddGroupItem(debug::Item::CreateBind("Far",		debug::Item::Float, &m_far));
	group->AddGroupItem(debug::Item::CreateBind("Aspect",	debug::Item::Float, &m_aspect));
	group->AddGroupItem(debug::Item::CreateBind("isShow",	debug::Item::Bool,	&m_isShow));
	window->AddItem(group);
}
void Camera::Draw()
{
	if (!m_isShow) return;
	DirectX::XMFLOAT3 look = GetLook();
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(look.x, look.y, look.z);
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f);
	DirectX::XMFLOAT4X4 mat;
	DirectX::XMStoreFloat4x4(&mat, DirectX::XMMatrixTranspose(S * T));
	Geometory::SetWorld(mat);
	Geometory::DrawBox();
}
#endif

DirectX::XMFLOAT4X4 Camera::GetView(bool transpose)
{
	DirectX::XMFLOAT3 pos = transform->GetPos();
	DirectX::XMFLOAT3 look = GetLook();
	DirectX::XMFLOAT3 up(0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR vPos	= DirectX::XMLoadFloat3(&pos);
	DirectX::XMVECTOR vLook	= DirectX::XMLoadFloat3(&look);
	DirectX::XMVECTOR vUp	= DirectX::XMLoadFloat3(&up);
	DirectX::XMMATRIX mat = DirectX::XMMatrixLookAtLH(vPos, vLook, vUp);
	if (transpose)
		mat = DirectX::XMMatrixTranspose(mat);
	DirectX::XMFLOAT4X4 fmat;
	DirectX::XMStoreFloat4x4(&fmat, mat);
	return fmat;
}

DirectX::XMFLOAT4X4 Camera::GetProj(bool transpose)
{
	DirectX::XMMATRIX mat;
	if (m_is3D) {
		mat = DirectX::XMMatrixPerspectiveFovLH(GetFovy(), m_aspect, m_near, m_far);
	}
	else {
		mat = DirectX::XMMatrixOrthographicLH(m_width, m_width / m_aspect, m_near, m_far);
	}
	if (transpose)
		mat = DirectX::XMMatrixTranspose(mat);
	DirectX::XMFLOAT4X4 fmat;
	DirectX::XMStoreFloat4x4(&fmat, mat);
	return fmat;
}
bool Camera::Is3D()
{
	return m_is3D;
}
float Camera::GetFovy()
{
	return DirectX::XMConvertToRadians(m_fovy);
}
float Camera::GetWidth()
{
	return m_width;
}
float Camera::GetNear()
{
	return m_near;
}
float Camera::GetFar()
{
	return m_far;
}
float Camera::GetAspect()
{
	return m_aspect;
}
float Camera::GetFocus()
{
	return m_focus;
}
DirectX::XMFLOAT3 Camera::GetLook()
{
	// 位置の取得
	DirectX::XMFLOAT3 pos = transform->GetPos();
	DirectX::XMVECTOR vPos = DirectX::XMLoadFloat3(&pos);

	// 前方ベクトル取得
	DirectX::XMFLOAT3 front = transform->GetFront();
	DirectX::XMVECTOR vFront = DirectX::XMLoadFloat3(&front);

	// カメラの位置からフォーカス距離まで進んだ位置を注視点とする
	vFront = DirectX::XMVectorScale(vFront, m_focus);
	DirectX::XMVECTOR vLook = DirectX::XMVectorAdd(vPos, vFront);

	DirectX::XMStoreFloat3(&pos, vLook);
	return pos;
}

void Camera::SetFocus(float focus)
{
	m_focus = focus;
}