#pragma once

#include <deque>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

#include <entidy/Entity.h>
#include <entidy/Exception.h>
#include <entidy/MemoryManager.h>
#include <entidy/Indexer.h>
#include <entidy/Iterator.h>
#include <entidy/Query.h>

namespace entidy
{
using namespace std;

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
		if(indexer->HasComponent(entity, key))
			throw EntidyException("Key: " + key +
								  " already exists for Entity: " + to_string(entity));

		Type* c = memory_manager->Pop<Type>(key);
		new(c) Type(std::forward<Args>(args)...);

		indexer->AddComponent(entity, key, (intptr_t)(c));
		return c;
	}

	template <typename Type>
	Type* Emplace(Entity entity, const string& key, const Type& component)
	{
		if(indexer->HasComponent(entity, key))
			throw EntidyException("Key: " + key +
								  " already exists for Entity: " + to_string(entity));

		Type* c = memory_manager->Pop<Type>(key);
		memcpy(c, &component, sizeof(Type));

		indexer->AddComponent(entity, key, (intptr_t)c);
		return c;
	}

	void Erase(Entity entity, const string& key)
	{
		if(!indexer->HasComponent(entity, key))
			throw EntidyException("Key: " + key +
								  " does not exist for Entity: " + to_string(entity));

		intptr_t component = indexer->GetComponent(entity, key);
		indexer->RemoveComponent(entity, key);
		memory_manager->Push(key, component);
	}

	void Erase(Entity entity)
	{
		if(!indexer->HasEntity(entity))
			throw EntidyException("Entity: " + to_string(entity) + " does not exist");

		unordered_map<string, intptr_t> components = indexer->GetAllComponents(entity);
		for(auto& it : components)
			memory_manager->Push(it.first, it.second);

		indexer->RemoveEntity(entity);
	}

	Query Select(const initializer_list<string>& keys)
	{
		return Query(indexer, keys);
	}

	template <typename... Args>
	Query Select(Args&&... args)
	{
		return Query(indexer, args...);
	}

	template <typename Type>
    Type * Component (Entity entity, const string &key)
    {
        if(!indexer->HasComponent(entity, key))
			throw EntidyException("Entity: " + to_string(entity) + " does not have component: " + key);

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