#ifndef __UNORDER_ACCESS_VIEW_H__
#define __UNORDER_ACCESS_VIEW_H__

#include "DirectX.h"

class UnorderedAccessView
{
public:
	UnorderedAccessView();
	~UnorderedAccessView();
	HRESULT Create(UINT stride, UINT num, void* pData = nullptr);

	void Copy();

	ID3D11UnorderedAccessView* GetUAV();
	ID3D11ShaderResourceView* GetSRV();

private:
	ID3D11Buffer* m_pBuffer;
	ID3D11UnorderedAccessView* m_pUAV;
	ID3D11ShaderResourceView* m_pSRV;
};

#endif