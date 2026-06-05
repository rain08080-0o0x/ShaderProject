#include "SceneLighting.h"
#include "Model.h"
#include "Camera.h"
#include "Light.h"
#include "Input.h"

void SceneLighting::Init()
{
	const char* file[] = {
		"VS_Object",
		"PS_TexColor",
		"PS_PointLight",	//点光源
		"PS_SpotLight",		//スポットライト
		"PS_RimLights",
		"PS_AllLightins",
	};
	Setup(file, _countof(file), 4);
	// スポットライトオブジェクトの作成
	GameObject* pSpotLight =
		CreateObj<GameObject>("SpotLight");
	// ライトコンポーネントの追加
	pSpotLight->AddComponent<Light>();

	for (int i = 0; i < 3; ++i)
	{
		char name[32];
		sprintf_s(name, sizeof(name), "RimLights%d", i);
		GameObject* pRimLight = CreateObj<GameObject>(name);
		pRimLight->AddComponent<Light>();
	}

	for (int i = 0; i < 5; ++i)
	{
		char name[32];
		sprintf_s(name, sizeof(name), "AllRimLight%d", i);
		GameObject* pARimLight = CreateObj<GameObject>(name);
		pARimLight->AddComponent<Light>();
	}


}
void SceneLighting::Uninit()
{
}
void SceneLighting::Update(float tick)
{
}
void SceneLighting::Draw()
{
	// ゲーム内のオブジェクトの取得
	GameObject* pModel[] = {
		GetObj<GameObject>("LightingModel0"),
		GetObj<GameObject>("LightingModel1"),
		GetObj<GameObject>("LightingModel2"),
		GetObj<GameObject>("LightingModel3"),
	};

	// カメラオブジェクトの取得
	GameObject* pCameraObj = GetObj<GameObject>("Camera");
	// カメラコンポーネントの取得
	Camera* pCameraComp = pCameraObj->GetComponent<Camera>();

	// ライトオブジェクトの取得
	GameObject* pLightObj = GetObj<GameObject>("Light");
	// ライトコンポーネントの取得
	Light* pLightComp = pLightObj->GetComponent<Light>();

	//点光源オブジェクトの取得
	GameObject* pPointLightObj = GetObj<GameObject>("Light");
	//点光源のライトコンポーネントを取得
	Light* pPLightComp = pPointLightObj->GetComponent<Light>();

	// スポットライトオブジェクトの取得
	GameObject* pSpotObj = GetObj<GameObject>("SpotLight");
	// ｽﾎﾟｯﾄﾗｲﾄのコンポーネント取得
	Light* pSpotComp = pSpotObj->GetComponent<Light>();

	// 読み込まれたシェーダーファイルの取得
	Shader* pVS = GetObj<Shader>("VS_Object");
	Shader* pPS = GetObj<Shader>("PS_TexColor");
	Shader* pPS_PointLight = GetObj<Shader>("PS_PointLight");
	Shader* pPS_Spot = GetObj<Shader>("PS_SpotLight");
	Shader* pPS_Rim = GetObj<Shader>("PS_RimLights");
	Shader* pPS_All = GetObj<Shader>("PS_AllLightins");

	// 定数バッファに渡す行列の情報を作成
	DirectX::XMFLOAT4X4 mat[3];
	mat[0] = pModel[0]->GetWorld(false);
	mat[1] = pCameraComp->GetView(false);
	mat[2] = pCameraComp->GetProj(false);

	// ライトの向き情報
	DirectX::XMFLOAT3 lightDir = pLightObj->GetFront();
	// 環境光
	DirectX::XMFLOAT4 ambient = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
#ifdef _DEBUG
	ambient = debug::Menu::Get("00_Info")["AmbientColor"].GetColor();
#endif
	// 定数バッファに渡すライトの情報
	DirectX::XMFLOAT4 lightParam[] = {
		// ライトの向き
		DirectX::XMFLOAT4(lightDir.x, lightDir.y, lightDir.z, 0.0f),
		// ライトの色
		pLightComp->GetDiffuse(),
		// 環境光
		ambient
	};

	// カメラの情報を定数バッファで渡す
	DirectX::XMFLOAT3 camPos = pCameraObj->GetPos();
	DirectX::XMFLOAT4 cameraParam[] = {
		{camPos.x, camPos.y, camPos.z, 0.0f}
	};

	//点光源の情報を定数バッファで渡す
	DirectX::XMFLOAT3 PointLightPos = pPointLightObj->GetPos();
	DirectX::XMFLOAT4 pLightParam[] = {
		{//点光源の位置
		PointLightPos.x,PointLightPos.y,PointLightPos.z		,
		pPLightComp->GetRange() * 5.0f		//点光源の範囲
		},
		pPLightComp->GetDiffuse(),	//点光源の色
	};

	// ｽﾎﾟｯﾄﾗｲﾄの情報を定数バッファで渡す
	DirectX::XMFLOAT3 SpotPos = pSpotObj->GetPos();
	DirectX::XMFLOAT3 SpotDir = pSpotObj->GetFront();
	DirectX::XMFLOAT4 spotPara[] =
	{
		{SpotPos.x,SpotPos.y,SpotPos.z,pSpotComp->GetRange() * 5.0f},
		pSpotComp->GetDiffuse(),
		{SpotDir.x,SpotDir.y,SpotDir.z,pSpotComp->GetSpotAngle()}
	};

	// リムライトの情報を定数バッファで渡す
	DirectX::XMFLOAT4 rimLightParam[] =
	{
		{PointLightPos.x, PointLightPos.y, PointLightPos.z, 0.0f},
		{lightDir.x, lightDir.y, lightDir.z, 0.0f},
		pLightComp->GetDiffuse(),
		ambient,
	};
	//========================================
	// リムライトの情報を定義
	//========================================
	//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
	const int RIM_LIGHT_MAX = 3;

	DirectX::XMFLOAT4 rimDir[RIM_LIGHT_MAX];
	DirectX::XMFLOAT4 rimColor[RIM_LIGHT_MAX];

	for (int i = 0; i < RIM_LIGHT_MAX; ++i)
	{
		char name[32];
		sprintf_s(name, sizeof(name), "RimLights%d", i);

		GameObject* pRimLightObj = GetObj<GameObject>(name);
		Light* pRimLightComp = pRimLightObj->GetComponent<Light>();

		DirectX::XMFLOAT3 dir = pRimLightObj->GetFront();

		rimDir[i] = DirectX::XMFLOAT4(dir.x, dir.y, dir.z, 0.0f);
		rimColor[i] = pRimLightComp->GetDiffuse();
		pRimLightComp->Draw();
	}
	DirectX::XMFLOAT4 PulsRimLightParam[] =
	{
		rimDir[0],
		rimDir[1],
		rimDir[2],

		rimColor[0],
		rimColor[1],
		rimColor[2],

		ambient,

		DirectX::XMFLOAT4(3.0f, 0.5f, 32.0f, 0.0f),
	};
	//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
	


	//========================================
	// AllLightings
	//========================================
	//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
	const int ARIM_LIGHT_MAX = 5;

	DirectX::XMFLOAT4 ArimDir[ARIM_LIGHT_MAX];
	DirectX::XMFLOAT4 ArimColor[ARIM_LIGHT_MAX];

	for (int i = 0; i < ARIM_LIGHT_MAX; ++i)
	{
		char name[32];
		sprintf_s(name, sizeof(name), "AllRimLight%d", i);

		GameObject* pRimLightObj = GetObj<GameObject>(name);
		Light* pRimLightComp = pRimLightObj->GetComponent<Light>();

		DirectX::XMFLOAT3 dir = pRimLightObj->GetFront();

		ArimDir[i] = DirectX::XMFLOAT4(dir.x, dir.y, dir.z, 0.0f);
		ArimColor[i] = pRimLightComp->GetDiffuse();
		pRimLightComp->Draw();
	}

	DirectX::XMFLOAT4 allLightParam[] =
	{
		// PhongLight
		DirectX::XMFLOAT4(PointLightPos.x, PointLightPos.y, PointLightPos.z, pPLightComp->GetRange() * 5.0f),
		DirectX::XMFLOAT4(pPLightComp->GetDiffuse().x, pPLightComp->GetDiffuse().y, pPLightComp->GetDiffuse().z, 1.0f),

		// AmbientLight
		DirectX::XMFLOAT4(ambient.x, ambient.y, ambient.z, 1.0f),

		// SpecularLight
		DirectX::XMFLOAT4(PointLightPos.x, PointLightPos.y, PointLightPos.z, 0.5f),
		DirectX::XMFLOAT4(pPLightComp->GetDiffuse().x, pPLightComp->GetDiffuse().y, pPLightComp->GetDiffuse().z, 64.0f),

		// SpotLight
		DirectX::XMFLOAT4(SpotPos.x, SpotPos.y, SpotPos.z, pSpotComp->GetRange() * 5.0f),
		DirectX::XMFLOAT4(SpotDir.x, SpotDir.y, SpotDir.z, pSpotComp->GetSpotAngle()),
		DirectX::XMFLOAT4(pSpotComp->GetDiffuse().x, pSpotComp->GetDiffuse().y, pSpotComp->GetDiffuse().z, 1.0f),

		// RimLight 5個
		ArimDir[0],
		ArimDir[1],
		ArimDir[2],
		ArimDir[3],
		ArimDir[4],

		ArimColor[0],
		ArimColor[1],
		ArimColor[2],
		ArimColor[3],
		ArimColor[4],

		// x = 使用数, y = 全体強度, z = 鋭さ, w = 未使用
		DirectX::XMFLOAT4(5.0f, 0.5f, 3.0f, 0.0f),
	};
	//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

	//--- シェーダーにデータ書き込み
	pPS_PointLight->WriteBuffer(0, pLightParam);
	// スポットライトの定数バッファ
	pPS_Spot->WriteBuffer(0, spotPara);
	pSpotComp->Draw();
	// リムライトの定数バッファ
	pPS_Rim->WriteBuffer(0, PulsRimLightParam);
	pPS_Rim->WriteBuffer(1, cameraParam);

	pPS_All->WriteBuffer(0, allLightParam);
	pPS_All->WriteBuffer(1, cameraParam);

	// モデル別のシェーダーを指定
	Shader* psList[] = {
		pPS_PointLight,
		pPS_Spot,
		pPS_Rim,
		pPS_All,
	};

	// モデルの描画
	for (int i = 0; i < _countof(pModel); ++i)
	{
		ModelRenderer* pRendererComp = pModel[i]->GetComponent<ModelRenderer>();
		Model* pDrawModel = pRendererComp->GetModel();
		if (pDrawModel) {
			// ワールド行列の設定
			mat[0] = pModel[i]->GetWorld(false);
			pVS->WriteBuffer(0, mat);
			// シェーダーを設定して描画
			pDrawModel->SetVertexShader(pVS);
			pDrawModel->SetPixelShader(psList[i]);
			pDrawModel->Draw();
		}
	}
}

