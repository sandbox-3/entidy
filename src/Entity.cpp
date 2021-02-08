#include <entidy/Entity.h>
#include <entidy/Registry.h>

using namespace entidy;

EntityImpl::EntityImpl(Registry parent, unsigned int id)
{
    this->parent = parent;
    this->id = id;
}

unsigned int EntityImpl::Id() const
{
    return id;
}

template <typename Type, typename... Args>
Type* EntityImpl::NewComponent(const string &key, Args &&...args)
{
    Type * comp = parent->NewComponent<Type>(this, key, args...);
    components.emplace(key, comp);
    return comp;
}

void EntityImpl::Delete()
{
    
}