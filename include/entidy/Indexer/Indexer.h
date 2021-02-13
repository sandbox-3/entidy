#pragma once
#include <unordered_map>

#include <roaring64map.hh>

#include <entidy/Entity.h>

#include <entidy/Indexer/Iterator.h>

namespace entidy
{
    using namespace roaring;
    
#ifdef ENTIDY_32_BIT
    using BitMap = CRoaring;
#else
    using BitMap = Roaring64Map;
#endif

    using namespace std;

    struct ComponentMap
    {
        BitMap entities;
        BitMap components;
    };

    class Indexer
    {
    protected:
        unordered_map<string, shared_ptr<ComponentMap>> index;

    public:
        Indexer() {}

        void AddKey(const string &key) {}
        void RemoveKey(const string &key) {}

        void AddEntity(Entity entity) {}
        void RemoveEntity(Entity entity) {}

        void AddComponent(Entity entity, const string &key, void *component) {}
        void RemoveComponent(Entity entity, const string &key) {}

        template <typename... Args>
        Iterator Fetch(const string &query, Args &&...args)
        {
            Iterator iterator;

            return iterator;
        }
    };

} // namespace entidy