#pragma once

#include <deque>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

#include <entidy/Entity.h>
#include <entidy/Indexer/Indexer.h>
#include <entidy/Indexer/Iterator.h>
#include <entidy/Registry.h>

namespace entidy
{
using namespace std;

class RegistryImpl;
using Registry = shared_ptr<RegistryImpl>;

class Query
{
protected:
	vector<string> select;
	Indexer indexer;

	Query(Indexer indexer, const initializer_list<string>& keys)
	{
		this->indexer = indexer;
		this->select = keys;
	}

	template <typename... Args>
	Query(Indexer indexer, Args... keys)
	{
		Query(indexer, initializer_list<string>{keys...});
	}

public:
	Iterator Filter(const string& filter = "")
	{
		return indexer->Fetch(select, filter);
	}

	friend RegistryImpl;
};

} // namespace entidy