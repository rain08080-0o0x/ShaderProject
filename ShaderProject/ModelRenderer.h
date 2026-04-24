#ifndef __MODEL_RENDERER_H__
#define __MODEL_RENDERER_H__

#include "Component.h"
#include "Model.h"

class ModelRenderer : public Component
{
public:
    ModelRenderer();
    ~ModelRenderer();
    void Execute() final;

    Model* GetModel();

    void ReadWrite(DataAccessor* data) final;
#if _DEBUG
    void Debug(debug::Window* window) final;
#endif

private:
    char m_modelName[MAX_PATH];
    Model* m_pModel;
    float m_scale;
};

#endif // __MODEL_RENDERER_H__