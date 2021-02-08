#include <entidy/Registry.h>

using namespace entidy;

RegistryImpl::RegistryImpl()
{
    internal_memory.Create<EntityImpl>("entity");
}

Entity RegistryImpl::NewEntity()
{
    Entity entity = internal_memory.Pop<EntityImpl>("entity");
    new (entity) EntityImpl(shared_from_this(), ++refCount);

    indexer.AddEntity(entity);
    return entity;
}

template <typename Type>
void RegistryImpl::RemoveComponent(Entity owner, const string &key, Type *component)
{
    indexer.RemoveComponent(owner, key);
    memory_manager.Push(key, component);
}

template <typename Type, typename... Args>
Type *RegistryImpl::NewComponent(Entity owner, const string &key, Args &&...args)
{
    if (!memory_manager.HasKey(key))
    {
        memory_manager.Create<Type>(key);
        indexer.AddKey(key);
    }

    Type *component = memory_manager.Pop<Type>(key);
    new (data) Type(std::forward<Args>(args)...);

    indexer.AddComponent(owner, key);
    return component;
}

const vector<Entity> &Fetch(Query query)
{
}