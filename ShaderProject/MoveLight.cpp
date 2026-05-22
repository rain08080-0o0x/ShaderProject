#include "MoveLight.h"
#include <algorithm>
#include "Input.h"
#include "Light.h"

MoveLight::MoveLight()
	: GameObject("MoveLight")
	, m_pComponent(AddComponent<Light>())
	, m_moveSpeed(1.0f)
{
}
MoveLight::~MoveLight()
{
}

void MoveLight::Update()
{
	if (IsKeyPress('L'))
	{
		const float rotSpeed = (360.0f / (2.0f * 60)) * m_moveSpeed; // “ñ•b‚Åˆê‰ñ“]
		float rotY = 0.0f;
		float rotXZ = 0.0f;
		if (IsKeyPress('A')) { rotY += rotSpeed; }
		if (IsKeyPress('D')) { rotY -= rotSpeed; }
		if (IsKeyPress('W')) { rotXZ += rotSpeed; }
		if (IsKeyPress('S')) { rotXZ -= rotSpeed; }

		m_rotation.x += rotXZ;
		m_rotation.y += rotY;
	}

	DirectX::XMFLOAT3 front = GetFront();
	DirectX::XMVECTOR vFront = DirectX::XMLoadFloat3(&front);
	DirectX::XMVECTOR vPos = DirectX::XMVectorScale(vFront, -m_pComponent->GetRange());
	DirectX::XMStoreFloat3(&m_pos, vPos);
}

#ifdef _DEBUG
void MoveLight::Debug(debug::Window* window)
{
	GameObject::Debug(window);

	debug::Item* group = debug::Item::CreateGroup("Move Light");
	group->AddGroupItem(debug::Item::CreateBind("MoveSpeed", debug::Item::Float, &m_moveSpeed));
	window->AddItem(group);
}
#endif
