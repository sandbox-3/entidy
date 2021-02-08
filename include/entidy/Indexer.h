#pragma once

#include <entidy/Entity.h>

namespace entidy
{
    using namespace std;

    class EntityImpl;
    using Entity = EntityImpl*;

    class Indexer
    {
    protected:
    public:
        Indexer();

        void AddKey(const string &key);
        void RemoveKey(const string &key);

        void AddEntity(Entity entity);
        void RemoveEntity(Entity entity);

        void AddComponent(Entity entity, const string &key);
        void RemoveComponent(Entity entity, const string &key);


    };

} // namespace entidy