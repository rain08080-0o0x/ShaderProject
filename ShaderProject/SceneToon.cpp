#include "SceneToon.h"
#include "Model.h"
#include "Camera.h"
#include "Light.h"
#include "Input.h"
#include "Defines.h"

void SceneToon::Init()
{
	const char* file[] = {
		"VS_Object",
		"PS_TexColor",
		"PS_Toon", // アニメ塗
		"PS_Outline",
		"VS_Outline",
		"PS_AlphaDiher_for",
	};
	Setup(file, _countof(file), 3);

	// ランプテクスチャの読み込み
	Texture* pRamp = CreateObj<Texture>("RampTex");
	pRamp->Create("Assets/Texture/RampTex.bmp");

#ifdef _DEBUG
	debug::Menu::Get("00_Info").AddItem(debug::Item::CreateBind("AlphaDither Alpha", debug::Item::Float, &m_alphaDitherAlpha));
	debug::Menu::Get("00_Info").AddItem(debug::Item::CreateBind("AlphaDither DistanceFade", debug::Item::Bool, &m_alphaDitherUseDistanceFade));
	debug::Menu::Get("00_Info").AddItem(debug::Item::CreateBind("AlphaDither FadeStart", debug::Item::Float, &m_alphaDitherFadeStart));
	debug::Menu::Get("00_Info").AddItem(debug::Item::CreateBind("AlphaDither FadeEnd", debug::Item::Float, &m_alphaDitherFadeEnd));
#endif
}
void SceneToon::Uninit()
{
}
void SceneToon::Update(float tick)
{
}
void SceneToon::Draw()
{
	// ゲーム内のオブジェクトの取得
	GameObject* pModel[] = {
		GetObj<GameObject>("ToonModel0"),
		GetObj<GameObject>("ToonModel1"),
		GetObj<GameObject>("ToonModel2"),
	};

	// テクスチャの取得
	Texture* pRampTex = GetObj<Texture>("RampTex");

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
	Shader* pPS_Toon = GetObj<Shader>("PS_Toon");
	Shader* pPS_OutLine = GetObj<Shader>("PS_Outline");
	Shader* pVS_Outline = GetObj<Shader>("VS_Outline");
	Shader* pPS_AlphaDiher = GetObj<Shader>("PS_AlphaDiher_for");

	// 定数バッファに渡す行列の情報を作成
	DirectX::XMFLOAT4X4 mat[3];
	mat[0] = pModel[0]->GetWorld(false);
	mat[1] = pCameraComp->GetView(false);
	mat[2] = pCameraComp->GetProj(false);

	DirectX::XMFLOAT3 Lpos = pLightObj->GetPos();
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
		DirectX::XMFLOAT4(Lpos.x,Lpos.y,Lpos.z,0.0f),
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

	float alphaDitherFadeEnd = m_alphaDitherFadeEnd;
	if (alphaDitherFadeEnd <= m_alphaDitherFadeStart)
	{
		alphaDitherFadeEnd = m_alphaDitherFadeStart + 0.001f;
	}

	// アルファディザシェーダーに渡すパラメータ
	DirectX::XMFLOAT4 AD_Param[] =
	{
		{SCREEN_WIDTH, SCREEN_HEIGHT, m_alphaDitherAlpha, m_alphaDitherUseDistanceFade ? 1.0f : 0.0f},
		{camPos.x, camPos.y, camPos.z, 0.0f},
		{m_alphaDitherFadeStart, alphaDitherFadeEnd, 0.0f, 0.0f},
	};

	//--- シェーダーにデータ書き込み
	pPS_Toon->WriteBuffer(0, lightParam);
	pPS_Toon->SetTexture(1, pRampTex);
	pPS_AlphaDiher->WriteBuffer(0, AD_Param);


	// 描画
	Shader* vsList[] =
	{
		pVS,
		pVS_Outline,
		pVS,
	};
	Shader* psList[] = {
		//pPS,
		pPS_Toon,
		pPS_OutLine,
		pPS_AlphaDiher,
	};
	for (int i = 0; i < _countof(pModel); ++i)
	{
		ModelRenderer* pRendererComp = pModel[i]->GetComponent<ModelRenderer>();
		Model* pDrawModel = pRendererComp->GetModel();
		if (pDrawModel) {
			// ワールド行列の設定
			mat[0] = pModel[i]->GetWorld(false);
			vsList[i]->WriteBuffer(0, mat);

			//カリングの設定
			if (i == 1)
			{
				SetCullingMode(D3D11_CULL_BACK);
				//pDrawModel->SetVertexShader(pVS_Outline);
			}
			else 
			{
				SetCullingMode(D3D11_CULL_FRONT);
				//pDrawModel->SetVertexShader(pVS);
			}

			pDrawModel->SetVertexShader(vsList[i]);
			pDrawModel->SetPixelShader( psList[i] );
			pDrawModel->Draw();
			SetCullingMode(D3D11_CULL_FRONT);
		}
	}
	SetCullingMode(D3D11_CULL_FRONT);
}
