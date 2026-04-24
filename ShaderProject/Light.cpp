#include "Light.h"
#include "Geometory.h"

Light::Light()
	: m_type(PointLight)
	, m_diffuse(1.0f, 1.0f, 1.0f, 1.0f)
	, m_range(1.0f)
	, m_spotAngle(60.0f)
#ifdef _DEBUG
	, m_isShow(false)
#endif
{
}
Light::~Light()
{
}
void Light::Execute()
{
}

Light::Type Light::GetType()
{
	return m_type;
}
DirectX::XMFLOAT4 Light::GetDiffuse()
{
	return m_diffuse;
}
float Light::GetRange()
{
	return m_range;
}
float Light::GetSpotAngle()
{
	return m_spotAngle;
}

void Light::SetType(Type type)
{
	m_type = type;
}
void Light::SetDiffuse(DirectX::XMFLOAT4 color)
{
	m_diffuse = color;
}
void Light::SetRange(float range)
{
	m_range = range;
}
void Light::SetSpotAngle(float angle)
{
	m_spotAngle = angle;
}

void Light::ReadWrite(DataAccessor* data)
{
	data->Access<Type>(&m_type);
	data->Access<DirectX::XMFLOAT4>(&m_diffuse);
	data->Access<float>(&m_range);
	data->Access<float>(&m_spotAngle);
#ifdef _DEBUG
	data->Access<bool>(&m_isShow);
#endif
}
#if _DEBUG
void Light::Debug(debug::Window* window)
{
	debug::Item* group = debug::Item::CreateGroup("Light Component");
	//DebugMenu::Item* list = DebugMenu::Item::CreateList("Type", )
	//group->AddGroupItem(Debug)
	group->AddGroupItem(debug::Item::CreateBind("Type",			debug::Item::Int, 		&m_type));
	group->AddGroupItem(debug::Item::CreateBind("Color",		debug::Item::Color,		&m_diffuse));
	group->AddGroupItem(debug::Item::CreateBind("Range",		debug::Item::Float,		&m_range));
	group->AddGroupItem(debug::Item::CreateBind("SpotAngle",	debug::Item::Float,		&m_spotAngle));
	group->AddGroupItem(debug::Item::CreateBind("isShow",		debug::Item::Bool,		&m_isShow));
	window->AddItem(group);
}
void Light::Draw()
{
	if (!m_isShow) return;
	DirectX::XMFLOAT3 pos = transform->GetPos();
	DirectX::XMFLOAT3 front = transform->GetFront();

	// 色設定
	Geometory::SetColor(m_diffuse);

	// オブジェクト位置を表すボックスの表示
	DirectX::XMFLOAT4X4 mat;
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(0.2f, 0.2f, 0.2f);
	DirectX::XMStoreFloat4x4(&mat, DirectX::XMMatrixTranspose(S * T));
	Geometory::SetWorld(mat);
	Geometory::DrawBox();

	// 光源方向の表示
	DirectX::XMFLOAT3 end(
		pos.x + front.x * m_range,
		pos.y + front.y * m_range,
		pos.z + front.z * m_range
	);
	Geometory::AddLine(pos, end);
	Geometory::DrawLines();
}
#endif