#pragma once

#include <deque>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

#include <entidy/Exception.h>
#include <entidy/Indexer.h>
#include <entidy/MemoryManager.h>
#include <entidy/Query.h>
#include <entidy/View.h>

namespace entidy
{
using namespace std;

using Entity = size_t;

class RegistryFactory;

class RegistryImpl;
using Registry = shared_ptr<RegistryImpl>;

class RegistryImpl : public enable_shared_from_this<RegistryImpl>
{
protected:
	MemoryManager memory_manager;
	Indexer indexer;

	RegistryImpl()
	{
		memory_manager = make_shared<MemoryManagerImpl>();
		indexer = make_shared<IndexerImpl>();
	}

public:
	Entity Create()
	{
		return indexer->AddEntity();
	}

	template <typename Type, typename... Args>
	Type* Emplace(Entity entity, const string& key, Args&&... args)
	{
		Type* c = memory_manager->Pop<Type>(key);
		new(c) Type(std::forward<Args>(args)...);

		intptr_t prev = indexer->AddComponent(entity, key, (intptr_t)(c));
		if(prev != 0)
			memory_manager->Push(key, prev);

		return c;
	}

	template <typename Type>
	Type* Emplace(Entity entity, const string& key, const Type& component)
	{
		Type* c = memory_manager->Pop<Type>(key);
		memcpy(c, &component, sizeof(Type));

		intptr_t prev = indexer->AddComponent(entity, key, (intptr_t)(c));
		if(prev != 0)
			memory_manager->Push(key, prev);
		return c;
	}

	void Erase(Entity entity, const string& key)
	{
		intptr_t component = indexer->RemoveComponent(entity, key);
		if(component != 0)
			memory_manager->Push(key, component);
	}

	void Erase(Entity entity)
	{
		vector<pair<string, intptr_t>> components = indexer->GetAllComponents(entity);
		for(auto& it : components)
			memory_manager->Push(it.first, it.second);
		indexer->RemoveEntity(entity);
	}

	bool Has(Entity entity, const string& key)
	{
		return indexer->HasComponent(entity, key);
	}

	Query Select(const initializer_list<string>& keys)
	{
		return Query(indexer, keys);
	}

	template <typename Type>
	Type* Component(Entity entity, const string& key)
	{
		return (Type*)indexer->GetComponent(entity, key);
	}

	void SizeHint(const string& key, size_t size_hint)
	{
		memory_manager->SizeHint(key, size_hint);
	}

	void CleanUp()
	{
		memory_manager->CleanUp();
		indexer->CleanUp();
	}

	friend RegistryFactory;
};

class RegistryFactory
{
public:
	static Registry New()
	{
		Registry registry(new RegistryImpl());
		return registry;
	}
};

} // namespace entidy