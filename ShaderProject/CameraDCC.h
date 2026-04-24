#ifndef __CAMERA_DCC_H__
#define __CAMERA_DCC_H__

#include "Camera.h"
#include "GameObject.hpp"
#include <Windows.h>

class CameraDCC : public GameObject
{
private:
	struct Argument
	{
		DirectX::XMFLOAT2 mouseMove;
		DirectX::XMVECTOR vCamFront;
		DirectX::XMVECTOR vCamSide;
		DirectX::XMVECTOR vCamUp;
		DirectX::XMVECTOR vCamPos;
		DirectX::XMVECTOR vCamLook;
		float speed;
	};
public:
	CameraDCC();
	~CameraDCC();
	void Update() final;

private:
	void UpdateState();
	void UpdateOrbit(Argument& arg);
	void UpdateTrack(Argument& arg);
	void UpdateDolly(Argument& arg);
	void UpdateFlight(Argument& arg);

private:
	int m_state;
	POINT m_oldPos;
	Camera* m_pComponent;
#ifdef _DEBUG
	char m_mode[256];
#endif
};

#endif