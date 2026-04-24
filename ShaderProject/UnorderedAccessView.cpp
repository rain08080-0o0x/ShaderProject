#include "UnorderedAccessView.h"


UnorderedAccessView::UnorderedAccessView()
    : m_pBuffer(nullptr)
    , m_pUAV(nullptr)
    , m_pSRV(nullptr)
{
}
UnorderedAccessView::~UnorderedAccessView()
{
    SAFE_RELEASE(m_pSRV);
    SAFE_RELEASE(m_pUAV);
    SAFE_RELEASE(m_pBuffer);
}
HRESULT UnorderedAccessView::Create(UINT stride, UINT num, void* pData)
{
    // https://wizframework.github.io/BaseCross64/15_01.html
    ID3D11Device* pDevice = GetDevice();
    ID3D11DeviceContext* pContext = GetContext();
    HRESULT hr = E_FAIL;

    // バッファ作成
    {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = stride * num;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
        desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // ComputeShaderで利用するための構造化バッファ
        desc.StructureByteStride = stride;
        D3D11_SUBRESOURCE_DATA data = {};
        data.pSysMem = pData;
        hr = pDevice->CreateBuffer(&desc, pData ? &data : nullptr, &m_pBuffer);
        if (FAILED(hr)) { return hr; }
    }

    // アクセスビュー作成
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        desc.Buffer.NumElements = num;
        hr = pDevice->CreateUnorderedAccessView(m_pBuffer, &desc, &m_pUAV);
        if (FAILED(hr)) { return hr; }
    }

    // シェーダーリソースビュー作成
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        desc.Buffer.FirstElement = 0;
        desc.Buffer.NumElements = num;
        hr = pDevice->CreateShaderResourceView(m_pBuffer, &desc, &m_pSRV);
        if (FAILED(hr)) { return hr; }
    }

    return hr;
}

void UnorderedAccessView::Copy()
{
    D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
    D3D11_BUFFER_DESC desc = {};
    D3D11_BUFFER_DESC base;
    m_pBuffer->GetDesc(&base);
    desc.ByteWidth = base.ByteWidth;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.StructureByteStride = base.StructureByteStride;
    ID3D11Buffer* pLocal;
    GetDevice()->CreateBuffer(&desc, nullptr, &pLocal);
    GetContext()->CopyResource(pLocal, m_pBuffer);
    if (SUCCEEDED(GetContext()->Map(pLocal, 0, D3D11_MAP_READ, 0, &MappedResource)))
    {
        float check[100];
        memcpy(check, MappedResource.pData, sizeof(check));
        int a = 0;
    }
}

ID3D11UnorderedAccessView* UnorderedAccessView::GetUAV()
{
    return m_pUAV;
}
ID3D11ShaderResourceView* UnorderedAccessView::GetSRV()
{
    return m_pSRV;
}