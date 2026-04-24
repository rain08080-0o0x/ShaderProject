#include "SceneBase.hpp"
#include "DebugMenu.h"
#include "GameObject.hpp"
#include <typeinfo>
#include <string.h>

SceneBase::Objects SceneBase::m_objects;
#ifdef _DEBUG
debug::Item* SceneBase::hierarchy;
debug::Item* SceneBase::inspector;
#endif

SceneBase::SceneBase(const char* name)
	: m_pParent(nullptr)
	, m_pSubScene(nullptr)
	, m_name(name)
{
}
SceneBase::~SceneBase()
{
	// サブシーンを削除
	RemoveSubScene();

	// 削除
	while (!m_items.empty())
	{
		DestroyObj(m_items.begin()->c_str());
	}
	m_items.clear();

	// 親の参照を削除
	if(m_pParent)
		m_pParent->m_pSubScene = nullptr;
}
void SceneBase::Initialize()
{
#ifdef _DEBUG
	debug::Menu::Create("Inspector");
	debug::Window& window = debug::Menu::Create("Hierarchy");
	hierarchy = debug::Item::CreateList("Name", [](const void* arg)
		{
			const char* name = reinterpret_cast<const char*>(arg);
			auto it = m_objects.find(name);
			if (it == m_objects.end()) return;
			auto& window = debug::Menu::Get("Inspector");
			window.Clear();
			window.AddItem(debug::Item::CreateValue(it->first.c_str(), debug::Item::Label));
			if (!it->second->isGameObject) return;
			
			static_cast<GameObject*>(it->second->m_pObj)->Debug(&window);
		});
	window.AddItem(hierarchy);
#endif
}
void SceneBase::_update(float tick)
{
	// シーンが所持しているオブジェクトの更新
	auto itemIt = m_items.begin();
	while (itemIt != m_items.end())
	{
		auto objIt = m_objects.find(*itemIt);
		// 型チェック
		if (objIt != m_objects.end() && objIt->second->isGameObject)
		{
			GameObject* obj = reinterpret_cast<GameObject*>(objIt->second->m_pObj);
			obj->Execute();
		}
		++itemIt;
	}

	// シーン自体の更新(クリア判定など
	Update(tick);

	// サブシーンの更新
	if (m_pSubScene)
		m_pSubScene->_update(tick);
}
void SceneBase::_draw()
{
	Draw();
	if (m_pSubScene)
		m_pSubScene->_draw();
}

/// @brief サブシーンの削除
void SceneBase::RemoveSubScene()
{
	// 削除するサブシーンが存在するか
	if (!m_pSubScene) return;

	// 階層内のサブシーンを優先して削除
	m_pSubScene->RemoveSubScene();

	// 直下のサブシーンを削除
	m_pSubScene->Uninit();

	delete m_pSubScene;
	m_pSubScene = nullptr;
}

void SceneBase::DestroyObj(const char* name)
{
	auto obj = m_objects.find(name);
	if (obj == m_objects.end()) return;

	delete obj->second;
	m_objects.erase(obj);

#ifdef _DEBUG
	debug::Window& hieralchy = debug::Menu::Get("Hierarchy");
	hierarchy->RemoveListItem(name);
#endif

	m_items.remove(name);
}

template<> GameObject* SceneBase::CreateObj(const char* name)
{
#ifdef _DEBUG
	// デバッグ中のみ、名称ダブりがないかチェック
	Objects::iterator it = m_objects.find(name);
	if (it != m_objects.end()) {
		static char buf[256];
		sprintf_s(buf, sizeof(buf), "Failed to create object. %s", name);
		MessageBox(NULL, buf, "Error", MB_OK);
		return nullptr;
	}
	// ヒエラルキーに追加
	hierarchy->AddListItem(name);
#endif // _DEBUG

	GameObject* ptr = new GameObject(name);
	m_objects.insert(std::pair<std::string, SceneObjectBase*>(name, new SceneObject<GameObject>(ptr)));
	m_items.push_back(name);
	return ptr;
}

void SceneBase::Setup(const char** shaderFiles, int shaderNum, int modelNum)
{
	for (int i = 0; i < shaderNum; ++i) {
		Shader* shader = nullptr;
		if (strstr(shaderFiles[i], "PS_") == shaderFiles[i]) {
			shader = CreateObj<PixelShader>(shaderFiles[i]);
		}
		else if (strstr(shaderFiles[i], "VS_") == shaderFiles[i]) {
			shader = CreateObj<VertexShader>(shaderFiles[i]);
		}
		else if (strstr(shaderFiles[i], "GS_") == shaderFiles[i]) {
			shader = CreateObj<GeometryShader>(shaderFiles[i]);
		}
		else if (strstr(shaderFiles[i], "HS_") == shaderFiles[i]) {
			shader = CreateObj<HullShader>(shaderFiles[i]);
		}
		else if (strstr(shaderFiles[i], "DS_") == shaderFiles[i]) {
			shader = CreateObj<DomainShader>(shaderFiles[i]);
		}
		else if (strstr(shaderFiles[i], "CS_") == shaderFiles[i]) {
			shader = CreateObj<ComputeShader>(shaderFiles[i]);
		}
		else {
			MessageBox(NULL, shaderFiles[i], "Shader name [VS_ / PS_]", MB_OK);
		}
		std::string path = "Assets/Shader/";
		path += shaderFiles[i];
		path += ".cso";
		if (shader && FAILED(shader->Load(path.c_str()))) {
			MessageBox(NULL, shaderFiles[i], "Shader Error", MB_OK);
		}
	}


	// 表示オブジェクト作成
	for (int i = 0; i < modelNum; ++i) {
		std::string name = m_name + "Model" + std::to_string(i);
		GameObject* obj = CreateObj<GameObject>(name.c_str());
		ModelRenderer* renderer = obj->AddComponent<ModelRenderer>();
	}
}