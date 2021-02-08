#pragma once

#include <unordered_map>
#include <deque>
#include <memory>
#include <string>

#include <entidy/MemoryManager/MemoryManager.h>

#include <entidy/Entity.h>
#include <entidy/Query.h>
#include <entidy/Indexer.h>

namespace entidy
{
    using namespace std;

    class RegistryFactory;

    class RegistryImpl;
    using Registry = shared_ptr<RegistryImpl>;

    class QueryImpl;
    using Query = QueryImpl*;

    class EntityImpl;
    using Entity = EntityImpl*;

    class RegistryImpl : public enable_shared_from_this<RegistryImpl>
    {
    protected:
        unsigned int refCount = 0;

        MemoryManager internal_memory;
        MemoryManager memory_manager;

        Indexer indexer;

        RegistryImpl();

        const vector<Entity>& Fetch(Query query);

        template <typename Type>
        void RemoveComponent(Entity owner, const string &key, Type* component);
        
        template <typename Type, typename ...Args>
        Type* NewComponent(Entity owner, const string &key, Args&&... args);
        
    public:
        Entity NewEntity();


        friend RegistryFactory;
        friend QueryImpl;
        friend EntityImpl;
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