#pragma once
#include <unordered_map>
#include <vector>

#include <entidy/Exception.h>
#include <entidy/QueryParser.h>
#include <entidy/SparseMap.h>
#include <entidy/View.h>
#include <entidy/CRoaring/roaring.hh>

namespace entidy
{

#ifdef ENTIDY_32_BIT
using BitMap = CRoaring;
#else
using BitMap = Roaring64Map;
#endif

using namespace std;

using Entity = size_t;

struct ComponentMap
{
	BitMap entities;
	SparseMap<intptr_t> components;
};

class RegistryImpl;
using Registry = shared_ptr<RegistryImpl>;

class IndexerImpl;
using Indexer = shared_ptr<IndexerImpl>;

class QPAdapter : public QueryParserAdapter<BitMap>
{
public:
	unordered_map<string, size_t>* index;

	QPAdapter(unordered_map<string, size_t>* index)
	{
		this->index = index;
	}

	virtual BitMap Evaluate(const string& token) override
	{
		size_t id = (*index)[token];
		BitMap map;
		map.add(id);
		cout << "WRONG: we need to return ComponentMap[c].entities" << endl;
		return map;
	}

	virtual BitMap And(const BitMap& lhs, const BitMap& rhs) override
	{
		return lhs & rhs;
	}

	virtual BitMap Or(const BitMap& lhs, const BitMap& rhs) override
	{
		return lhs | rhs;
	}

	virtual BitMap Not(const BitMap& rhs) override
	{
		auto copy = rhs;
		copy.flip(0, copy.maximum());
		return copy;
	}
};

class IndexerImpl : public enable_shared_from_this<IndexerImpl>
{

protected:
	vector<Entity> entity_pool;
	Entity entityRefCount = 0;

	vector<size_t> component_pool;
	size_t componentRefCount = 0;

	unordered_map<string, size_t> index;
	vector<shared_ptr<ComponentMap>> maps;

	MemoryManager memory_manager;

	size_t NewComponent(const string& key)
	{
		size_t c;
		if(component_pool.size() > 0)
		{
			c = component_pool.back();
			component_pool.pop_back();
			index[key] = c;
		}
		else
		{
			c = componentRefCount++;
			maps.emplace_back(make_shared<ComponentMap>());
			index.emplace(key, c);
		}
		return c;
	}

	size_t ComponentIndex(const string& key, bool auto_create = true)
	{
		auto it = index.find(key);
		if(it != index.end())
			return it->second;

		if(auto_create)
			return NewComponent(key);

		throw(EntidyException("Key: " + key + " does not exist"));
	}

	string ComponentKey(size_t id)
	{
		for(auto& it : index)
		{
			if(it.second == id)
				return it.first;
		}

		throw(EntidyException("Component Id: " + to_string(id) + " does not exist"));
	}

public:
	IndexerImpl()
	{
		memory_manager = make_shared<MemoryManagerImpl>();
	}

	bool HasEntity(Entity e)
	{
		if(entityRefCount <= e)
			return false;

		for(auto& it : entity_pool)
			if(it == e)
				return false;

		return true;
	}

	Entity AddEntity()
	{
		Entity entity;
		if(entity_pool.size() > 0)
		{
			entity = entity_pool.back();
			entity_pool.pop_back();
		}
		else
		{
			entity = entityRefCount++;
		}
		return entity;
	}

	void RemoveEntity(Entity entity)
	{
		for(auto& map : maps)
		{
			map->entities.remove(entity);
			map->components.Remove(entity);
		}

		entity_pool.push_back(entity);
	}

	bool HasComponent(Entity e, const string& key)
	{
		size_t c = ComponentIndex(key);
		return maps[c]->entities.contains(e);
	}

	void AddComponent(Entity entity, const string& key, intptr_t component)
	{
		size_t c = ComponentIndex(key);
		maps[c]->entities.add(entity);
		maps[c]->components.Emplace(entity, component);
	}

	void RemoveComponent(Entity entity, const string& key)
	{
		size_t c = ComponentIndex(key);
		maps[c]->entities.remove(entity);
		maps[c]->components.Remove(entity);
	}

	intptr_t GetComponent(Entity entity, const string& key)
	{
		size_t c = ComponentIndex(key);
		return maps[c]->components.Select(entity);
	}

	unordered_map<string, intptr_t> GetAllComponents(Entity entity)
	{
		unordered_map<string, intptr_t> out;
		for(size_t i = 0; i < maps.size(); i++)
		{
			auto map = maps[i];
			if(map->entities.contains(entity))
				out.emplace(ComponentKey(i), map->components.Select(entity));
		}
		return out;
	}

	void CleanUp()
	{
		auto it = index.begin();
		while(it != index.end())
		{
			auto map = maps[it->second];
			if(map->entities.cardinality() == 0)
			{
				it = index.erase(it);
				component_pool.push_back(it->second);
			}
			map->entities.shrinkToFit();
		}
	}

	View Fetch(const vector<string>& keys, const string& filter)
	{
		QueryParser<BitMap> qp(make_shared<QPAdapter>(&index));
		BitMap query = qp.Parse(filter);

		size_t rows = query.cardinality();
		size_t cols = keys.size() + 1;
		vector<PagedVector<1024>> results;
		results.push_back(PagedVector<1024>(memory_manager));

		cout << query.cardinality() << endl;
		auto it = query.begin();
		size_t i = 0;
		while(it != query.end())
		{
			results[0].Write(i++, *it);
			++it;
		}

		for(size_t i = 0; i < keys.size(); i++)
		{
			results.push_back(PagedVector<1024>(memory_manager));
			size_t c = ComponentIndex(keys[i], false);
			maps[c]->components.Select(results[i + 1], query.begin(), query.end());
		}

		return View(results);
	}
};

} // namespace entidy