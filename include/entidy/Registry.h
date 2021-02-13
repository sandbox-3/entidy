#pragma once

#include <unordered_map>
#include <deque>
#include <memory>
#include <string>
#include <tuple>

#include <entidy/Entity.h>
#include <entidy/MemoryManager.h>
#include <entidy/Indexer/Indexer.h>
#include <entidy/Indexer/Iterator.h>

namespace entidy
{
    using namespace std;

    class RegistryFactory;

    class RegistryImpl;
    using Registry = shared_ptr<RegistryImpl>;

    class RegistryImpl : public enable_shared_from_this<RegistryImpl>
    {
    protected:
        vector<Entity> entities{1000};
        Entity refCount = 0;

        MemoryManager memory_manager;
        Indexer indexer;

        RegistryImpl()
        {
        }

    public:
        Entity NewEntity()
        {
            Entity entity;
            if (entities.size() > 0)
            {
                entity = entities.back();
                entities.pop_back();
            }
            entity = ++refCount;

            indexer.AddEntity(entity);
            return entity;
        }

        void DeleteEntity(Entity entity)
        {
            indexer.RemoveEntity(entity);
        }

        void DeleteComponent(Entity owner, const string &key, void *component)
        {
            indexer.RemoveComponent(owner, key);
            memory_manager.Push(key, component);
        }

        template <typename Type, typename... Args>
        Type *NewComponent(Entity owner, const string &key, Args &&...args)
        {
            if (!memory_manager.HasKey(key))
                indexer.AddKey(key);

            Type *component = memory_manager.Pop<Type>(key);
            new (data) Type(std::forward<Args>(args)...);

            indexer.AddComponent(owner, key, component);
            return component;
        }

        template <typename... Args>
        Iterator Fetch(const string &query, Args &&...args)
        {
            return indexer.Fetch<Args...>(query, args...);
        }

        friend RegistryFactory;
    };

    class RegistryFactory
    {
    public:
        static Registry NewRegistry()
        {
            Registry registry(new RegistryImpl());
            return registry;
        }
    };

} // namespace entidy