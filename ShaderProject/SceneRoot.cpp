#include "SceneRoot.h"
#include <stdio.h>
#include "CameraDCC.h"
#include "MoveLight.h"
#include "Input.h"
#include "Geometory.h"
#include "DebugMenu.h"
#include "ModelRenderer.h"

#include "SceneShading.h"
#include "SceneLighting.h"
#include "SceneBumpmap.h"


void SceneRoot::Init()
{
	// オブジェクトの作成
	CameraDCC* pCamera = CreateObj<CameraDCC>("Camera");
	MoveLight* pLight = CreateObj<MoveLight>("Light");
	
#ifdef _DEBUG
	// グリッド表示
	debug::Item* camera = debug::Item::CreateGroup("Grid");
	camera->AddGroupItem(debug::Item::CreateValue("Enable",	debug::Item::Bool,	true));
	camera->AddGroupItem(debug::Item::CreateValue("Size",	debug::Item::Float, true));
	camera->AddGroupItem(debug::Item::CreateValue("Margin",	debug::Item::Float, true));
	camera->AddGroupItem(debug::Item::CreateValue("Color",	debug::Item::Color, true));
	camera->AddGroupItem(debug::Item::CreateValue("Axis",	debug::Item::Bool,	true));
	debug::Menu::Get("00_Info").AddItem(camera);

	// シーン表示
	debug::Window& scene = debug::Menu::Create("Scene");
	debug::Item* list = debug::Item::CreateList("Scenes", [this](const void* arg) {
		RemoveSubScene();
		const char* name = reinterpret_cast<const char*>(arg);
		if (strcmp(name, "Shading") == 0) AddSubScene<SceneShading>();
		if (strcmp(name, "Lighting") == 0) AddSubScene<SceneLighting>();
		if (strcmp(name, "BumpMap") == 0) AddSubScene<SceneBumpmap>();
	}, true);
	list->AddListItem("Shading");
	list->AddListItem("Lighting");
	list->AddListItem("BumpMap");
	scene.AddItem(list);
#else
	// リリース時の開始シーン
	AddSubScene<SceneShading>();
#endif
}

void SceneRoot::Uninit()
{
}

void SceneRoot::Update(float tick)
{
}
void SceneRoot::Draw()
{
#ifdef _DEBUG
	Camera* pCamera = GetObj<CameraDCC>("Camera")->GetComponent<Camera>();
	Light* pLight = GetObj<MoveLight>("Light")->GetComponent<Light>();

	Geometory::SetView(pCamera->GetView());
	Geometory::SetProjection(pCamera->GetProj());
	pCamera->Draw();
	pLight->Draw();

	debug::Window& window = debug::Menu::Get("00_Info");
	if (window["Grid"]["Enable"].GetBool())
	{
		DirectX::XMFLOAT4X4 fmat;
		DirectX::XMStoreFloat4x4(&fmat, DirectX::XMMatrixIdentity());
		Geometory::SetWorld(fmat);

		// 網掛け描画
		float GridSize = window["Grid"]["Size"].GetFloat();
		GridSize *= 0.5f;
		float GridMargin = window["Grid"]["Margin"].GetFloat();
		DirectX::XMFLOAT4 color;
		color = window["Grid"]["Color"].GetColor();
		Geometory::SetColor(color);
		float d = GridMargin;
		float s = GridSize;
		while (s >= GridMargin && GridMargin > 0.0f)
		{
			Geometory::AddLine(DirectX::XMFLOAT3( d, 0.0f, -GridSize), DirectX::XMFLOAT3( d, 0.0f, GridSize));
			Geometory::AddLine(DirectX::XMFLOAT3(-d, 0.0f, -GridSize), DirectX::XMFLOAT3(-d, 0.0f, GridSize));
			Geometory::AddLine(DirectX::XMFLOAT3(-GridSize, 0.0f,  d), DirectX::XMFLOAT3(GridSize, 0.0f,  d));
			Geometory::AddLine(DirectX::XMFLOAT3(-GridSize, 0.0f, -d), DirectX::XMFLOAT3(GridSize, 0.0f, -d));
			d += GridMargin;
			s -= GridMargin;
		}
		// 軸描画
		if (window["Grid"]["Axis"].GetBool())
		{
			Geometory::SetColor(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
			Geometory::AddLine(DirectX::XMFLOAT3(-GridSize, 0.0f, 0.0f), DirectX::XMFLOAT3(GridSize, 0.0f, 0.0f));
			Geometory::SetColor(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
			Geometory::AddLine(DirectX::XMFLOAT3(0.0f, -GridSize, 0.0f), DirectX::XMFLOAT3(0.0f, GridSize, 0.0f));
			Geometory::SetColor(DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
			Geometory::AddLine(DirectX::XMFLOAT3(0.0f, 0.0f, -GridSize), DirectX::XMFLOAT3(0.0f, 0.0f, GridSize));
		}
		else
		{
			Geometory::AddLine(DirectX::XMFLOAT3(-GridSize, 0.0f, 0.0f), DirectX::XMFLOAT3(GridSize, 0.0f, 0.0f));
			Geometory::AddLine(DirectX::XMFLOAT3(0.0f, 0.0f, -GridSize), DirectX::XMFLOAT3(0.0f, 0.0f, GridSize));
		}
		Geometory::DrawLines();
	}
#endif
}
