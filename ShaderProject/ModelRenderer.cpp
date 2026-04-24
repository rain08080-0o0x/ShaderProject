#include "ModelRenderer.h"


ModelRenderer::ModelRenderer()
    : m_pModel(nullptr)
    , m_modelName{}
    , m_scale(1.0f)
{
}
ModelRenderer::~ModelRenderer()
{
    if(m_pModel)
        delete m_pModel;
}
void ModelRenderer::Execute()
{
    if (m_pModel) return;
    m_pModel = new Model();
    m_pModel->Load(m_modelName, m_scale);
}

Model* ModelRenderer::GetModel()
{
    return m_pModel;
}

void ModelRenderer::ReadWrite(DataAccessor* data)
{
    size_t size = strlen(m_modelName);
    data->Access<size_t>(&size);
    for (size_t i = 0; i < size; ++i)
        data->Access<char>(&m_modelName[i]);
    m_modelName[size] = '\0';
}
#ifdef _DEBUG
void ModelRenderer::Debug(debug::Window* window)
{
    auto group = debug::Item::CreateGroup("Model Renderer");
    group->AddGroupItem(debug::Item::CreateBind("Path",     debug::Item::Path,  m_modelName));
    group->AddGroupItem(debug::Item::CreateBind("Scale",    debug::Item::Float, &m_scale));
    group->AddGroupItem(debug::Item::CreateCallBack("Load", debug::Item::Command,
        [this](bool isWrite, void* arg) {
            delete m_pModel;
            m_pModel = nullptr;
		}));
    window->AddItem(group);
}
#endif