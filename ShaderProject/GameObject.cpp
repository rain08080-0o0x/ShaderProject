#include "GameObject.hpp"
#include "Component.h"
#include <typeinfo>
#include <algorithm>

/*
* @brief コンストラクタ
* @param[in] name オブジェクト名
*/
GameObject::GameObject(std::string name)
	: m_name(name)
	, m_pos{}, m_rotation{0.0f, 0.0f, 0.0f}, m_scale{1.0f, 1.0f, 1.0f}
{
	// オブジェクト名に応じて、保存ファイルの読み込み
	std::string path = "Assets/GameObject/" + m_name + ".dat";
	FILE* fp;
	fopen_s(&fp, path.c_str(), "rb");
	if (fp) {
		// ファイル一括読み込み
		fseek(fp, 0, SEEK_END);
		long fileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		char* ptr = new char[fileSize];
		fread(ptr, fileSize, 1, fp);
		m_datas.push_back({"data", ptr}); // 一括で読み込んだデータを保存
		fclose(fp);

		// ゲームオブジェクト内のデータの読み込み
		memcpy(&m_pos, ptr, sizeof(m_pos));
		memcpy(&m_rotation, ptr += sizeof(m_pos), sizeof(m_rotation));
		memcpy(&m_scale, ptr += sizeof(m_rotation), sizeof(m_scale));
		ptr += sizeof(m_scale);
		// データのキーと値が保存されている個所へのポインタを取得（コンポーネントの領域を確保
		while (ptr - m_datas[0].value < fileSize)
		{
			char* data[2]; // キー,値
			size_t size = -1;
			for (int i = 0; i < 2; ++i)
			{
				// データサイズ
				size = *reinterpret_cast<size_t*>(ptr);
				if ((ptr - m_datas[0].value) + size >= fileSize) {
					ptr += fileSize;
					size = -1;
					break;
				}
				ptr += sizeof(size);
				// データ
				data[i] = ptr;
				ptr += size;
			}
			// 異常値チェック
			if(size != -1)
				m_datas.push_back({ data[0], data[1] });
		}
	}
}

/*
* @brief デストラクタ
*/
GameObject::~GameObject()
{
	// 保存データの削除
	if(!m_datas.empty())
		delete[] m_datas[0].value;

	auto it = m_components.begin();

#ifdef _DEBUG
	// データの保存
	std::string path = "Assets/GameObject/" + m_name + ".dat";
	FILE* fp;
	fopen_s(&fp, path.c_str(), "wb");
	if (fp)
	{
		// ゲームオブジェクトのデータを保存
		fwrite(&m_pos, sizeof(m_pos), 1, fp);
		fwrite(&m_rotation, sizeof(m_rotation), 1, fp);
		fwrite(&m_scale, sizeof(m_scale), 1, fp);

		// コンポーネントのデータを保存
		it = m_components.begin();
		while (it != m_components.end())
		{
			const char* name = typeid(**it).name();
			Component::DataAccessor accessor(nullptr);
			(*it)->ReadWrite(&accessor);
			// データのキーを保存
			size_t size = strlen(name);
			fwrite(&size, sizeof(size), 1, fp);
			fwrite(name, size, 1, fp);
			// データの保存
			size = accessor.GetWriteSize();
			fwrite(&size, sizeof(size), 1, fp);
			fwrite(accessor.GetData(), size, 1, fp);
			++it;
		}
		fclose(fp);
	}
#endif

	// コンポーネントの削除
	it = m_components.begin();
	while (it != m_components.end())
	{
		delete (*it);
		++it;
	}
}

/*
* @brief 更新処理
*/
void GameObject::Execute()
{
	// コンポーネントの処理
	auto it = m_components.begin();
	while (it != m_components.end())
	{
		(*it)->Execute();
		++it;
	}
	// 継承先オブジェクトの処理
	Update();
}

#ifdef _DEBUG
/*
* @brief インスペクターへの表示
*/
void GameObject::Debug(debug::Window* window)
{
	// トランスフォームグループの作成
	debug::Item* group = debug::Item::CreateGroup("Transform");
	group->AddGroupItem(debug::Item::CreateBind("Pos",		debug::Item::Vector, &m_pos));
	group->AddGroupItem(debug::Item::CreateBind("Rotation", debug::Item::Vector, &m_rotation));
	group->AddGroupItem(debug::Item::CreateBind("Scale",	debug::Item::Vector, &m_scale));
	window->AddItem(group);

	// コンポーネントのインスペクター登録
	auto it = m_components.begin();
	while (it != m_components.end())
	{
		(*it)->Debug(window);
		++it;
	}
}
#endif

/*
* @brief 座標設定
*/
void GameObject::SetPos(DirectX::XMFLOAT3 pos)
{
	m_pos = pos;
}

/*
* @brief 回転の設定
*/
void GameObject::SetRotation(DirectX::XMFLOAT3 angle)
{
	m_rotation = angle;
}

/*
* @brief 行列から回転値の設定
*/
void GameObject::SetRotation(const DirectX::XMFLOAT3X3& mat)
{
	m_rotation.y = asinf(mat._13);
	float c = cosf(m_rotation.y);
	if (fabsf(c) <= FLT_EPSILON)
	{
		m_rotation.x = atanf(mat._32 / mat._22);
		m_rotation.z = 0.0f;
	}
	else
	{
		m_rotation.x = atanf(-mat._23 / mat._33);
		m_rotation.z = atanf(-mat._12 / mat._11);
	}

}

/*
* @brief 座標取得
*/
DirectX::XMFLOAT3 GameObject::GetPos()
{
	return m_pos;
}
/*
* @brief ワールド行列取得
* @param[in] transpose 転置設定
*/
DirectX::XMFLOAT4X4 GameObject::GetWorld(bool transpose)
{
	// 各要素の行列を取得
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	DirectX::XMMATRIX R = GetRotationMatrix();
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	// 行列の合算
	DirectX::XMMATRIX M = S * R * T;
	// 転置
	if (transpose)
		M = DirectX::XMMatrixTranspose(M);
	// XMMATRIXからXMFLOATへ変換
	DirectX::XMFLOAT4X4 fMat;
	DirectX::XMStoreFloat4x4(&fMat, M);

	return fMat;
}
/*
* @brief 回転行列の取得
*/
DirectX::XMMATRIX GameObject::GetRotationMatrix()
{
	return 
		DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(m_rotation.x))*
		DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(m_rotation.y))*
		DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(m_rotation.z));
}
/*
* @brief 回転値の取得
*/
DirectX::XMFLOAT3 GameObject::GetRotation()
{
	return m_rotation;
}
/*
* @brief 前方ベクトルの取得
*/
DirectX::XMFLOAT3 GameObject::GetFront()
{
	DirectX::XMVECTOR vFront = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMMATRIX matR = GetRotationMatrix();
	vFront = DirectX::XMVector3TransformCoord(vFront, matR);
	DirectX::XMFLOAT3 dir;
	DirectX::XMStoreFloat3(&dir, DirectX::XMVector3Normalize(vFront));
	return dir;
}

/*
* @brief 右方向ベクトルの取得
*/
DirectX::XMFLOAT3 GameObject::GetRight()
{
	DirectX::XMVECTOR vRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX matR = GetRotationMatrix();
	vRight = DirectX::XMVector3TransformCoord(vRight, matR);
	DirectX::XMFLOAT3 dir;
	DirectX::XMStoreFloat3(&dir, DirectX::XMVector3Normalize(vRight));
	return dir;
}
/*
* @brief 上方ベクトルの取得
*/
DirectX::XMFLOAT3 GameObject::GetUp()
{
	DirectX::XMVECTOR vUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX matR = GetRotationMatrix();
	vUp = DirectX::XMVector3TransformCoord(vUp, matR);
	DirectX::XMFLOAT3 dir;
	DirectX::XMStoreFloat3(&dir, vUp);
	return dir;
}

/*
* @brief コンポーネント追加時の初期化処理
*/
void GameObject::_addComponent(Component* component)
{
	// 所持オブジェクトの登録
	component->transform = this;

	// 保存データに一致するコンポーネントがあるか探索
	const char* name = typeid(*component).name();
	auto it = std::find_if(m_datas.begin(), m_datas.end(), [&name](SaveData& data) {
		return strstr(data.name, name) == data.name;
		});
	if (it == m_datas.end()) return;

	// 保存されている情報を設定
	Component::DataAccessor accessor(it->value);
	component->ReadWrite(&accessor);
}