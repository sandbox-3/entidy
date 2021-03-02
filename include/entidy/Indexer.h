#pragma once
#include <typeinfo>
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
	size_t type;
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

	/**
     * @brief Creates a new component with key 'key'.
     * This function also handles recycling component indices.
     * @param key The key for the requested component.
     * @return The index of the newly created component.
     */
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

	/**
     * @brief Returns the index of the component with key 'key'.
     * If the component does not exist, it is created.
     * @param key The key for the requested component.
     * @return The index of the component.
     */
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

	/**
     * @brief Returns a new or recycled entity.
     * @return Entity.
     */
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

	/**
     * @brief Removes an entity and deletes all its components.
     * @param entity The entity to remove.
     */
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

	/**
     * @brief Checks if entity 'entity' has a component at key 'key'.
     * @param entity The entity to check.
     * @param key The key for the component to check.
     */
	bool HasComponent(Entity entity, const string& key)
	{
		size_t c = ComponentIndex(key);
		return maps[c].entities.contains(entity);
	}

	/**
     * @brief Creates, indexes and returns a memory-managed instance of component.
     * The new instance is allocated or recycled by the memory pool.
     * If the component key does not exist, it is created and a Type association is saved.
     * @tparam Type The component type.
     * @param entity The entity.
     * @param key The key for for the component to add.
     * @return A pointer to the created component.
     * @throw EntidyException if the key had been previously used for a different type.
     */
	template <typename Type>
	Type* CreateComponent(Entity entity, const string& key)
	{
		size_t c = ComponentIndex(key);

		if(!maps[c].mem_pool)
			maps[c].mem_pool = MemoryManagerImpl::Create<Type>();

		if(maps[c].type == 0)
			maps[c].type = typeid(Type*).hash_code();

		if(maps[c].type != typeid(Type*).hash_code())
			throw(EntidyException("Component Type mismatch for key " + key));

		intptr_t prev = maps[c].components->Read(entity);
		if(prev != 0)
			maps[c].mem_pool->Push(prev);

		Type* cur = maps[c].mem_pool->Pop<Type>();
		maps[c].components->Write(entity, (intptr_t)cur);
		maps[c].entities.add(entity);
		return cur;
	}

	/**
     * @brief Deletes component with key 'key' for entity 'entity'.
     * Also sends the instance back to the memory-manager for recycling.
     * @param entity The entity.
     * @param key The key for for the component to delete.
     * @return false if component was not found, true otherwise.
     */
	bool DeleteComponent(Entity entity, const string& key)
	{
		size_t c = ComponentIndex(key);
		maps[c].entities.remove(entity);
		intptr_t prev = maps[c].components->Erase(entity);
		if(prev != 0 && maps[c].mem_pool)
			maps[c].mem_pool->Push(prev);
		return prev != 0;
	}

	/**
     * @brief Returns a pointer to the component with key 'key' for entity 'entity'.
     * NULL values are possible if 'entity' does not have a component for 'key'
     * @tparam Type The component type.
     * @param entity The entity.
     * @param key The key for for the requested component.
     * @return A pointer of type 'Type' to the component at 'key'.
     * @throw EntidyException if the provided Type does not match with the type associated with 'key'.
     */
	template <typename Type>
	Type* GetComponent(Entity entity, const string& key)
	{
		size_t c = ComponentIndex(key);
		if(maps[c].type != typeid(Type*).hash_code())
			throw(EntidyException("Component Type mismatch for key " + key));
		return (Type*)maps[c].components->Read(entity);
	}

	/**
     * @brief Performs a query and copies lists of pointers to the requested components.
     * @param keys The ordered list of components requested. Empty list is allowed.
     * @param filter A query string to filter by.
     * @return A View with lists of pointers to the requested components.
     * @throw EntidyException if the filter string has a syntax error or is empty.
     */
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

		vector<size_t> types(keys.size() + 1);
		types[0] = typeid(Entity).hash_code();
		for(size_t k = 0; k < keys.size(); k++)
		{
			i = 0;
			size_t c = ComponentIndex(keys[k]);

			SparseVector<DEFAULT_SV_SIZE> sv = maps[c].components;
			types[k + 1] = maps[c].type;

			it = query.begin();

			while(it != query.end())
			{
				results[k + 1]->Write(i, sv->Read(*it));
				++i;
				++it;
			}
		}

		return View(results, types);
	}

	/**
     * @brief Removes empty component pools and deallocates their reserved memory.
     * Removes orphaned entities that have no components attached to them.
     * Optimizes the bitmaps for faster queries and reduced memory consumption.
     * Should be called infrequently, and only when too many temporary dynamic components were created and deleted.
     */
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

		// TODO: Remove orphaned entities
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
		copy.flip(0, componentRefCount);
		return copy;
	}
};

} // namespace entidy