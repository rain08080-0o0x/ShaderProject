#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include <DirectXMath.h>
#include <string>
#include <vector>
#include "DebugMenu.h"

class Component;

class GameObject
{
private:
	// コンポーネントリスト
	using Components = std::vector<Component*>;

	// データの保存
	struct SaveData
	{
		const char* name;	// 保存する値の名称
		char* value;		// 保存値
	};
	using Datas = std::vector<SaveData>;
public:
	GameObject(std::string name);
	virtual ~GameObject();
	void Execute();

	// コンポーネントの生成
	template<class T>
	T* AddComponent();
	// コンポーネントの取得
	template<class T>
	T* GetComponent();

#ifdef _DEBUG
	// Inspectorへの表示登録
	virtual void Debug(debug::Window* window);
#endif

	void SetPos(DirectX::XMFLOAT3 pos);
	void SetRotation(DirectX::XMFLOAT3 angle);
	void SetRotation(const DirectX::XMFLOAT3X3& mat);
	DirectX::XMFLOAT3 GetPos();
	DirectX::XMFLOAT4X4 GetWorld(bool transpose = true);
	DirectX::XMMATRIX GetRotationMatrix();
	DirectX::XMFLOAT3 GetRotation();
	DirectX::XMFLOAT3 GetFront();
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetUp();
protected:
	// 継承先のクラスでオブジェクト別の処理を実装する場合、上書きすること。
	virtual void Update() {}
private:
	// コンポーネント追加時に型に関係なく呼び出す処理
	void _addComponent(Component* component);

private:
	Components			m_components;	// コンポーネントの一覧
	Datas				m_datas;		// 保存データ
	std::string			m_name;			// オブジェクト名
protected:
	DirectX::XMFLOAT3	m_pos;		// 座標
	DirectX::XMFLOAT3	m_rotation;	// 回転
	DirectX::XMFLOAT3	m_scale;	// 拡縮
};


/*
* @brief コンポーネントの追加
*/
template<class T>
T* GameObject::AddComponent()
{
#ifdef _DEBUG
	// デバッグ時のみ、指定された型がComponentを継承しているか確認
	static_assert(std::is_base_of<Component, T>(),
		"[GameObject::GetComponent] template T does not inherit from 'Component'");
#endif
	// コンポーネント生成
	T* ptr = new T;
	// 型に関係ない初期化処理を実施
	_addComponent(ptr);
	// 管理リストに追加
	m_components.push_back(ptr);

	return ptr;
}

/*
* @brief コンポーネントの取得
*/
template<class T>
T* GameObject::GetComponent()
{
	T* ptr = nullptr;
	auto it = m_components.begin();
	while (it != m_components.end())
	{
		// 型チェック
		if (typeid(T) == typeid(**it))
		{
			ptr = reinterpret_cast<T*>(*it);
			break;
		}
		++it;
	}
	return ptr;
}

#endif // __GAME_OBJECT_H__