#include "SceneBumpmap.h"
#include "Model.h"
#include "Camera.h"
#include "Light.h"
#include "Input.h"
#include "Sprite.h"


//--- プロトタイプ宣言
void CalcTangent(Model::RemakeInfo& data);

// バンプマップ用の頂点データ
struct TangentVtx {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 tangent; // ワールド空間上でテクスチャを貼る際の方向
};


void SceneBumpmap::Init()
{
	const char* file[] = {
		"VS_Object",
		"PS_TexColor",
		"VS_Bumpmap",// バンプマップの頂点シェーダー
		"PS_Bumpmap",// バンプマップのピクセルシェーダー
	};
	Setup(file, _countof(file), 1);

	// 使用する法線マップの読み込み
	Texture* pNormalMap = CreateObj<Texture>("NormalMap");
	pNormalMap->Create("Assets/Model/plane/normal.png");

	// バンプマップを適用するモデルを直接読み込み
	Model* pPlane = CreateObj<Model>("Plane");
	pPlane->Load("Assets/Model/plane/plane.fbx");
	pPlane->RemakeVertex(sizeof(TangentVtx), CalcTangent);

	Texture* pHeieghtMap = CreateObj<Texture>("HightMap");
	pHeieghtMap->Create("Assets/Model/plane/height.png");
}
void SceneBumpmap::Uninit()
{
}
void SceneBumpmap::Update(float tick)
{
}
void SceneBumpmap::Draw()
{
	// ゲーム内オブジェクト
	GameObject* pModel[] = {
		GetObj<GameObject>("BumpmapModel0"),
	};	

	// 読み込んだテクスチャの取得
	Texture* pNormalMap = GetObj<Texture>("NormalMap");
	Texture* pHeightMap = GetObj<Texture>("HeightMap");
	// モデルデータ取得M
	Model* pPlane = GetObj<Model>("Plane");

	// カメラオブジェクトの取得
	GameObject* pCameraObj = GetObj<GameObject>("Camera");
	// カメラコンポーネントの取得
	Camera* pCameraComp = pCameraObj->GetComponent<Camera>();

	// ライトオブジェクトの取得
	GameObject* pLightObj = GetObj<GameObject>("Light");
	// ライトコンポーネントの取得
	Light* pLightComp = pLightObj->GetComponent<Light>();


	// 読み込まれたシェーダーファイルの取得
	Shader* pVS = GetObj<Shader>("VS_Object");
	Shader* pPS = GetObj<Shader>("PS_TexColor");
	Shader* pVS_Bumpmap = GetObj<Shader>("VS_Bumpmap");
	Shader* pPS_Bumpmap = GetObj<Shader>("PS_Bumpmap");

	// 定数バッファに渡す行列の情報を作成
	DirectX::XMFLOAT4X4 mat[3];
	mat[0] = pModel[0]->GetWorld(false);
	mat[1] = pCameraComp->GetView(false);
	mat[2] = pCameraComp->GetProj(false);

	// ライトの位置情報
	DirectX::XMFLOAT3 lightPos = pLightObj->GetPos();

	// ライトの向き情報
	DirectX::XMFLOAT3 lightDir = pLightObj->GetFront();
	// 環境光
	DirectX::XMFLOAT4 ambient = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
#ifdef _DEBUG
	ambient = debug::Menu::Get("00_Info")["AmbientColor"].GetColor();
#endif
	// 定数バッファに渡すライトの情報
	DirectX::XMFLOAT4 lightParam[] = {
		// ライトの位置
		DirectX::XMFLOAT4(lightPos.x,lightPos.y,lightPos.z,0.0f),
		// ライトの向き
		DirectX::XMFLOAT4(lightDir.x, lightDir.y, lightDir.z, 0.0f),
		// ライトの色
		pLightComp->GetDiffuse(),
		// 環境光
		//ambient
	};

	// カメラの情報を定数バッファで渡す
	DirectX::XMFLOAT3 camPos = pCameraObj->GetPos();
	DirectX::XMFLOAT4 cameraParam[] = {
		{camPos.x, camPos.y, camPos.z, 0.0f}
	};

	pVS->WriteBuffer(0, mat);
	pPS->WriteBuffer(0, lightParam);
	pPS->WriteBuffer(1, cameraParam);
	// Planeのワールド行列を設定
	DirectX::XMStoreFloat4x4(
		&mat[0],
		DirectX::XMMatrixTranspose(
		DirectX::XMMatrixScaling(3,3,3))
	);

	//--- シェーダーにデータ書き込み
	pVS_Bumpmap->WriteBuffer(0, mat);
	pPS_Bumpmap->WriteBuffer(0, lightParam);
	pPS_Bumpmap->SetTexture(1, pNormalMap);
	pPS_Bumpmap->SetTexture(2, pHeightMap);

	// 読み込んだモデルを直接表示
	pPlane->SetVertexShader(pVS_Bumpmap);
	pPlane->SetPixelShader(pPS_Bumpmap);
	pPlane->Draw();

	// モデル別のシェーダーを指定
	Shader* psList[] = {
		pPS
	};

	// モデルの描画
	for (int i = 0; i < _countof(pModel); ++i)
	{
		ModelRenderer* pRendererComp = pModel[i]->GetComponent<ModelRenderer>();
		Model* pDrawModel = pRendererComp->GetModel();
		if (pDrawModel) {
			// ワールド行列の設定
			mat[0] = pModel[i]->GetWorld(false);
			pVS_Bumpmap->WriteBuffer(0, mat);
			// シェーダーを設定して描画
			pDrawModel->SetVertexShader(pVS);
			pDrawModel->SetPixelShader(psList[i]);
			pDrawModel->Draw();
		}
	}
}


void CalcTangent(Model::RemakeInfo& data)
{
	// 既存データ(接ベクトル以外)は共通なので値のコピーを行う
	TangentVtx* destVtx = reinterpret_cast<TangentVtx*>(data.dest);
	const Model::Vertex* srcVtx = reinterpret_cast<const Model::Vertex*>(data.source);
	for (UINT i = 0; i < data.vtxNum; ++i)
	{
		destVtx[i].pos = srcVtx[i].pos;
		destVtx[i].uv = srcVtx[i].uv;
		destVtx[i].normal = srcVtx[i].normal;
		destVtx[i].tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	}

	//--- 接ベクトルの計算
	// 計算途中では１頂点に接ベクトルが複数含まれる
	using TanVecAry = std::vector<DirectX::XMFLOAT3>;
	// それが、頂点数分必要
	std::vector<TanVecAry> tangentVtx;
	tangentVtx.resize(data.vtxNum);
	// 接ベクトルの計算単位は三角形
	// 三角形を構成するインデックスを取得
	const UINT* idx = reinterpret_cast<const UINT*>(data.idx);

	// １面(三頂点、三角形)ずつ処理
	for (UINT i = 0; i < data.idxNum; i += 3)
	{
		// 三角形を構成する頂点情報の取得
		UINT idx0 = idx[i + 0];
		UINT idx1 = idx[i + 1];
		UINT idx2 = idx[i + 2];
		// 頂点
		DirectX::XMFLOAT3 p[] = {
			destVtx[idx0].pos,
			destVtx[idx1].pos,
			destVtx[idx2].pos
		};
		// UV
		DirectX::XMFLOAT2 uv[] = {
			destVtx[idx0].uv,
			destVtx[idx1].uv,
			destVtx[idx2].uv
		};
		// 頂点同士を結ぶベクトル
		DirectX::XMFLOAT3 V[] = {
			{p[1].x - p[0].x, p[1].y - p[0].y, p[1].z - p[0].z},
			{p[2].x - p[0].x, p[2].y - p[0].y, p[2].z - p[0].z},
		};
		// UV同士を結ぶベクトル
		DirectX::XMFLOAT2 ST[] = {
			{uv[1].x - uv[0].x, uv[1].y - uv[0].y},
			{uv[2].x - uv[0].x, uv[2].y - uv[0].y},
		};

		//--- 接ベクトルの計算
		float factor = ST[0].x * ST[1].y - ST[0].y * ST[1].x;
		DirectX::XMFLOAT3 T(
			(ST[1].y * V[0].x - ST[0].y * V[1].x) / factor,
			(ST[1].y * V[0].y - ST[0].y * V[1].y) / factor,
			(ST[1].y * V[0].z - ST[0].y * V[1].z) / factor
		);

		// 接ベクトルの正規化
		float length = sqrtf(T.x * T.x + T.y * T.y + T.z * T.z);
		T.x /= length;
		T.y /= length;
		T.z /= length;
		// 頂点に接ベクトルの情報を追加
		tangentVtx[idx0].push_back(T);
		tangentVtx[idx1].push_back(T);
		tangentVtx[idx2].push_back(T);
	}

	// 頂点に割り当てられた接ベクトルの平均を求める
	for (UINT i = 0; i < data.vtxNum; ++i)
	{
		DirectX::XMFLOAT3 total(0.0f, 0.0f, 0.0f);
		auto it = tangentVtx[i].begin();
		while (it != tangentVtx[i].end())
		{
			total.x += it->x;
			total.y += it->y;
			total.z += it->z;
			++it;
		}
		// 合計から平均を求める
		total.x /= tangentVtx[i].size();
		total.y /= tangentVtx[i].size();
		total.z /= tangentVtx[i].size();
		// 平均結果を該当頂点の接ベクトルとする
		destVtx[i].tangent = total;
	}
}