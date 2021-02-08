#pragma once

#include <memory>
#include <unordered_map>
#include <string>

#include <entidy/Registry.h>

namespace entidy
{
    using namespace std;

    class RegistryImpl;
    using Registry = shared_ptr<RegistryImpl>;

    class EntityImpl;
    using Entity = EntityImpl*;

    class EntityImpl
    {
    protected:
        Registry parent;
        unsigned int id;

        unordered_map<string, void*> components;

        EntityImpl(Registry parent, unsigned int id);

    public:
        unsigned int Id() const;
        void Delete();

        template <typename Type, typename ...Args>
        Type* NewComponent(const string &key, Args&&... args);
        
        friend RegistryImpl;
    };

} // namespace entidy