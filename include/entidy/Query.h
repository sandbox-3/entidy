#pragma once

#include <unordered_set>
#include <string>
#include <vector>
#include <algorithm>

#include <entidy/Registry.h>
#include <entidy/Entity.h>

namespace entidy
{
    using namespace std;

    class EntityImpl;
    using Entity = EntityImpl*;

    class QueryImpl;
    using Query = QueryImpl*;

    class RegistryImpl;
    using Registry = shared_ptr<RegistryImpl>;

    class QueryImpl
    {
    protected:
        shared_ptr<RegistryImpl> parent;

        unordered_set<string> included_keys;
        unordered_set<string> excluded_keys;

        QueryImpl(shared_ptr<RegistryImpl> parent);
        QueryImpl(shared_ptr<RegistryImpl> parent, const string &single_key);
        QueryImpl(shared_ptr<RegistryImpl> parent, const initializer_list<string> &included_keys, const initializer_list<string> &excluded_keys);

    public:
        const Query And(const string &key);
        const Query Not(const string &key);
        bool Match(Entity entity) const;
        vector<Entity> Fetch();
        string Hash() const;

        friend RegistryImpl;
    };

} // namespace entidy