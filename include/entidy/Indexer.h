#pragma once
#include <unordered_map>
#include <vector>

#include <entidy/CRoaring/roaring.hh>
#include <entidy/Exception.h>
#include <entidy/MemoryManager.h>
#include <entidy/QueryParser.h>
#include <entidy/SparseVector.h>
#include <entidy/View.h>

namespace entidy
{

using BitMap = Roaring;

using namespace std;

using Entity = uint32_t;

struct ComponentMap
{
	BitMap entities;
	SparseVector<DEFAULT_SV_SIZE> components;
	MemoryManager mem_pool;
};

class IndexerImpl;
using Indexer = shared_ptr<IndexerImpl>;

class IndexerImpl : public enable_shared_from_this<IndexerImpl>, public QueryParserAdapter<BitMap>
{

protected:
	vector<Entity> entity_pool;
	Entity entityRefCount = 1;

	vector<size_t> component_pool;
	size_t componentRefCount = 0;

	unordered_map<string, size_t> index;
	vector<ComponentMap> maps;

	MemoryManager sv_mem_pool;

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
			maps.emplace_back(ComponentMap());
			maps.back().components = make_shared<SparseVectorImpl<DEFAULT_SV_SIZE>>(sv_mem_pool);
			index.emplace(key, c);
		}
		return c;
	}

	size_t ComponentIndex(const string& key)
	{
		auto it = index.find(key);
		if(it != index.end())
			return it->second;

		return NewComponent(key);
	}

public:
	IndexerImpl()
	{
		sv_mem_pool = MemoryManagerImpl::Create<Page<DEFAULT_SV_SIZE>>();
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
			map.entities.remove(entity);
			intptr_t prev = map.components->Erase(entity);
			if(prev != 0 && map.mem_pool)
				map.mem_pool->Push(prev);
		}

		entity_pool.push_back(entity);
	}

	bool HasComponent(Entity e, const string& key)
	{
		size_t c = ComponentIndex(key);
		return maps[c].entities.contains(e);
	}

	template <typename Type>
	Type* CreateComponent(Entity entity, const string& key)
	{
		size_t c = ComponentIndex(key);

        if(!maps[c].mem_pool)
            maps[c].mem_pool = MemoryManagerImpl::Create<Type>();
        
		intptr_t prev = maps[c].components->Read(entity);
		if(prev != 0)
			maps[c].mem_pool->Push(prev);

        Type * cur = maps[c].mem_pool->Pop<Type>();
		maps[c].components->Write(entity, (intptr_t)cur);
		maps[c].entities.add(entity);
		return cur;
	}

	intptr_t DeleteComponent(Entity entity, const string& key)
	{
		size_t c = ComponentIndex(key);
		maps[c].entities.remove(entity);
		intptr_t prev = maps[c].components->Erase(entity);
		if(prev != 0 && maps[c].mem_pool)
			maps[c].mem_pool->Push(prev);
		return prev;
	}

	intptr_t GetComponent(Entity entity, const string& key)
	{
		size_t c = ComponentIndex(key);
		return maps[c].components->Read(entity);
	}

	void CleanUp()
	{
		auto it = index.begin();
		while(it != index.end())
		{
			auto map = maps[it->second];
			if(map.entities.cardinality() == 0)
			{
				component_pool.push_back(it->second);
				it = index.erase(it);
			}
			else
			{
				++it;
			}
			map.entities.shrinkToFit();
		}
	}

	View Fetch(const vector<string>& keys, const string& filter)
	{
		QueryParser<BitMap> qp(this);
		BitMap query;

		if(filter == "")
			throw(EntidyException("No filter set"));

		query = qp.Parse(filter);
		for(auto k : keys)
			query &= Evaluate(k);

		vector<SparseVector<DEFAULT_SV_SIZE>> results;
		for(size_t k = 0; k < keys.size() + 1; k++)
			results.push_back(make_shared<SparseVectorImpl<DEFAULT_SV_SIZE>>(sv_mem_pool));

		size_t i = 0;
		auto it = query.begin();
		while(it != query.end())
		{
			results[0]->Write(i, *it);
			++i;
			++it;
		}

		for(size_t k = 0; k < keys.size(); k++)
		{
			i = 0;
			it = query.begin();
			while(it != query.end())
			{
				size_t c = ComponentIndex(keys[k]);
				results[k + 1]->Write(i, maps[c].components->Read(*it));
				++i;
				++it;
			}
		}

		return View(results);
	}

	// Query Parser Adapter Functions

	virtual BitMap Evaluate(const string& token) override
	{
		size_t id = ComponentIndex(token);
		return maps[id].entities;
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
		auto copy = BitMap(rhs);
		copy.flip(0, copy.maximum());
		return copy;
	}
};

} // namespace entidy