#include "Component.h"

Component::Component()
    : transform(nullptr)
{
}
Component::~Component()
{
}
void Component::Execute()
{
}
void Component::ReadWrite(DataAccessor* data)
{
}
#ifdef _DEBUG
void Component::Debug(debug::Window* window)
{
}
#endif

