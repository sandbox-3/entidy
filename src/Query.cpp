#include <entidy/Query.h>

using namespace entidy;

QueryImpl::QueryImpl(shared_ptr<RegistryImpl> parent)
{
    this->parent = parent;
}

QueryImpl::QueryImpl(shared_ptr<RegistryImpl> parent, const string &single_key)
{
    this->parent = parent;
    this->included_keys.emplace(single_key);
}

QueryImpl::QueryImpl(shared_ptr<RegistryImpl> parent, const initializer_list<string> &included_keys, const initializer_list<string> &excluded_keys)
{
    this->parent = parent;
    for (auto &v : included_keys)
        this->included_keys.emplace(v);
    for (auto &v : excluded_keys)
        this->excluded_keys.emplace(v);
}

const Query QueryImpl::And(const string &key)
{
    this->included_keys.emplace(key);
    return this;
}

const Query QueryImpl::Not(const string &key)
{
    this->excluded_keys.emplace(key);
    return this;
}

string QueryImpl::Hash() const
{
    // std::sort(std::begin(included_keys), std::end(included_keys));
    // std::sort(std::begin(excluded_keys), std::end(excluded_keys));

    // string hash = "";
    // for(auto & s: included_keys)
    //     hash += "::" + s;
    // for(auto & s: excluded_keys)
    //     hash += "::" + s;

    // return hash;
}

bool QueryImpl::Match(Entity entity) const
{
}

vector<Entity> QueryImpl::Fetch()
{
}
