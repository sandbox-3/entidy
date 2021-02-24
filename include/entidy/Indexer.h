#pragma once
#include <unordered_map>
#include <vector>

#include <entidy/CRoaring/roaring.hh>
#include <entidy/Exception.h>
#include <entidy/QueryParser.h>
#include <entidy/View.h>
#include <entidy/SparseVector.h>

namespace entidy
{

#ifdef ENTIDY_32_BIT
using BitMap = Roaring;
#else
using BitMap = Roaring64Map;
#endif

using namespace std;

using Entity = size_t;

struct ComponentMap
{
	BitMap entities;
	SparseVector<1024> components;
};

class RegistryImpl;
using Registry = shared_ptr<RegistryImpl>;

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
			maps.emplace_back(ComponentMap());
			maps.back().components = make_shared<SparseVectorImpl<1024>>(memory_manager);
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
			map.entities.remove(entity);
			map.components->Erase(entity);
		}

		entity_pool.push_back(entity);
	}

	bool HasComponent(Entity e, const string& key)
	{
		size_t c = ComponentIndex(key);
		return maps[c].entities.contains(e);
	}

	intptr_t AddComponent(Entity entity, const string& key, intptr_t component)
	{
		size_t c = ComponentIndex(key);
        intptr_t prev = maps[c].components->Read(entity);
		maps[c].components->Write(entity, component);
		maps[c].entities.add(entity);
        return prev;
	}

	intptr_t RemoveComponent(Entity entity, const string& key)
	{
		size_t c = ComponentIndex(key);
        intptr_t prev = maps[c].components->Read(entity);
		maps[c].components->Erase(entity);
		maps[c].entities.remove(entity);
        return prev;
	}

	intptr_t GetComponent(Entity entity, const string& key)
	{
		size_t c = ComponentIndex(key);
		return maps[c].components->Read(entity);
	}

	vector<pair<string, intptr_t>> GetAllComponents(Entity entity)
	{
		vector<pair<string, intptr_t>> out;
        for(auto &it : index)
        {
			auto &map = maps[it.second];
            intptr_t ptr = map.components->Read(entity);
            if(ptr != 0)
                out.push_back(pair(it.first, ptr));
        }
		return out;
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

		vector<SparseVector<1024>> results;
		for(size_t k = 0; k < keys.size() + 1; k++)
			results.push_back(make_shared<SparseVectorImpl<1024>>(memory_manager));

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