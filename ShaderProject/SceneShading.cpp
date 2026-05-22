#include "SceneShading.h"
#include "Model.h"
#include "Camera.h"
#include "Light.h"
#include "Input.h"

void SceneShading::Init()
{
	const char* file[] = {
		"VS_Object",
		"PS_TexColor",
		"PS_Phong", // 自作したファイルの指定
		"PS_Specular", // 自作したファイルの指定
	};
	Setup(file, _countof(file), 3);
#ifdef _DEBUG
	debug::Menu::Get("00_Info").AddItem(debug::Item::CreateBind("SpecularAmbientPower", debug::Item::Float, &m_specularAmbientPower));
#endif
}
void SceneShading::Uninit()
{
}
void SceneShading::Update(float tick)
{
}
void SceneShading::Draw()
{
	// ゲーム内のオブジェクトの取得
	GameObject* pModel[] = {
		GetObj<GameObject>("ShadingModel0"),
		GetObj<GameObject>("ShadingModel1"),
		GetObj<GameObject>("ShadingModel2"),
	};

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
	Shader* pPS_Phong = GetObj<Shader>("PS_Phong");
	Shader* pPS_Specular = GetObj<Shader>("PS_Specular");

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

	// ここで鏡面反射用に光源の位置を渡す為のデータ用意------
	DirectX::XMFLOAT3 lightPos = pLightObj->GetPos();
	DirectX::XMFLOAT3 camPos = pCameraObj->GetPos();

	DirectX::XMFLOAT4 phongParam[] = {
		DirectX::XMFLOAT4(lightPos.x, lightPos.y, lightPos.z, 0.0f),
		pLightComp->GetDiffuse(),
		ambient,
	};

	DirectX::XMFLOAT4 specularAmbient = DirectX::XMFLOAT4(
		ambient.x * m_specularAmbientPower,
		ambient.y * m_specularAmbientPower,
		ambient.z * m_specularAmbientPower,
		ambient.w);

	DirectX::XMFLOAT4 specularParam[] = {
		DirectX::XMFLOAT4(lightPos.x, lightPos.y, lightPos.z, 0.0f),
		pLightComp->GetDiffuse(),
		specularAmbient,
		DirectX::XMFLOAT4(camPos.x, camPos.y, camPos.z, 0.0f),
	};
	// ここまで----------------------------------------------

	// カメラの情報を定数バッファで渡す
	camPos = pCameraObj->GetPos();
	DirectX::XMFLOAT4 cameraParam[] = {
		{camPos.x, camPos.y, camPos.z, 0.0f}
	};

	// シェーダーにデータ書き込み
	pVS->WriteBuffer(0, mat);
	pPS->WriteBuffer(0, lightParam);
	pPS_Phong->WriteBuffer(0, phongParam);
	pPS_Specular->WriteBuffer(0, specularParam);

	// 描画
	for (int i = 0; i < _countof(pModel); ++i)
	{
		if (!pModel[i]) continue;

		mat[0] = pModel[i]->GetWorld(false);
		pVS->WriteBuffer(0, mat);

		switch (i)
		{
		case 0:pPS->WriteBuffer(0, lightParam); break;
		case 1:pPS_Phong->WriteBuffer(0, phongParam); break;
		case 2:pPS_Specular->WriteBuffer(0, specularParam); break;
		}

		ModelRenderer* pRendererComp = pModel[i]->GetComponent<ModelRenderer>();
		Model* pDrawModel = pRendererComp->GetModel();
		if (pDrawModel) {
			pDrawModel->SetVertexShader(pVS);
			//pDrawModel->SetPixelShader(pPS);
			//pDrawModel->SetPixelShader(pPS_Phong);
			//pDrawModel->SetPixelShader(pPS_Specular);

			switch (i)
			{
			case 0:pDrawModel->SetPixelShader(pPS); break;
			case 1:pDrawModel->SetPixelShader(pPS_Phong); break;
			case 2:pDrawModel->SetPixelShader(pPS_Specular); break;
			}
			pDrawModel->Draw();
		}
	}
}

