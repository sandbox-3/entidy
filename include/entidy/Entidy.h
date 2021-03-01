#pragma once

#include <deque>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

#include <entidy/Exception.h>
#include <entidy/Indexer.h>
#include <entidy/Query.h>
#include <entidy/View.h>

namespace entidy
{
using namespace std;

using Entity = uint32_t;

class Entidy
{
protected:
	Indexer indexer;


public:
	Entidy()
	{
		indexer = make_shared<IndexerImpl>();
	}

	~Entidy()
	{
	}

	Entity Create()
	{
		return indexer->AddEntity();
	}

	template <typename Type, typename... Args>
	Type* Emplace(Entity entity, const string& key, Args&&... args)
	{
		Type* c = indexer->CreateComponent<Type>(entity, key);
		new(c) Type(std::forward<Args>(args)...);
        
		return c;
	}

	template <typename Type>
	Type* Emplace(Entity entity, const string& key, const Type& component)
	{
		Type* c = indexer->CreateComponent<Type>(entity, key);
		memcpy(c, &component, sizeof(Type));

		return c;
	}

	bool Erase(Entity entity, const string& key)
	{
		intptr_t prev = indexer->DeleteComponent(entity, key);
		return prev != 0;
	}

	void Erase(Entity entity)
	{
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
        // TODO : Size Hints
	}

	void CleanUp()
	{
		indexer->CleanUp();
	}
};
} // namespace entidy