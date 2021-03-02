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

	~Entidy() { }

	/**
     * @brief Returns a new or recycled entity.
     * @return Entity.
     */
	Entity Create()
	{
		return indexer->AddEntity();
	}

	/**
     * @brief Creates, indexes and returns a memory-managed instance of component.
     * The new instance is allocated or recycled by the memory pool.
     * If the component key does not exist, it is created and a Type association is saved.
     * @tparam Type The component type.
     * @param entity The entity.
     * @param key The key for for the component to add.
     * @param args... The arguments to forward to Type's constructor.
     * @return A pointer to the created component.
     * @throw EntidyException if the key had been previously used for a different type.
     */
	template <typename Type, typename... Args>
	Type* Emplace(Entity entity, const string& key, Args&&... args)
	{
		Type* c = indexer->CreateComponent<Type>(entity, key);
		new(c) Type(std::forward<Args>(args)...);
		return c;
	}

	/**
     * @brief Creates, indexes and returns a memory-managed instance of component.
     * The new instance is allocated or recycled by the memory pool.
     * If the component key does not exist, it is created and a Type association is saved.
     * The provided component will be copied into the newly created component.
     * WARNING: The provided component to copy must NOT contain non memory-managed heap-allocated data. 
     * @tparam Type The component type.
     * @param entity The entity.
     * @param key The key for for the component to add.
     * @param component The component that will be copied into the newly created component.
     * @return A pointer to the created component.
     * @throw EntidyException if the key had been previously used for a different type.
     */
	template <typename Type>
	Type* Emplace(Entity entity, const string& key, const Type& component)
	{
		Type* c = indexer->CreateComponent<Type>(entity, key);
		memcpy(c, &component, sizeof(Type));

		return c;
	}

	/**
     * @brief Removes an entity and deletes all its components.
     * Sends all the deleted components to the memory-manager for recycling.
     * @param entity The entity to remove.
     */
	void Erase(Entity entity)
	{
		indexer->RemoveEntity(entity);
	}

	/**
     * @brief Deletes component with key 'key' for entity 'entity'.
     * Also sends the instance back to the memory-manager for recycling.
     * @param entity The entity.
     * @param key The key for for the component to delete.
     * @return false if component was not found, true otherwise.
     */
	bool Erase(Entity entity, const string& key)
	{
		return indexer->DeleteComponent(entity, key);
	}

	/**
     * @brief Checks if Entity 'entity' has a component with key 'key'.
     * @param entity The entity.
     * @param key The key for for the component to check.
     * @return false if component was not found, true otherwise.
     */
	bool Has(Entity entity, const string& key)
	{
		return indexer->HasComponent(entity, key);
	}

	/**
     * @brief Returns a Query object that is pre-built with a list of component keys to fetch.
     * @param keys A list of keys to fetch. Empty lists are allowed.
     * @return A Query object with the list of requested components.
     */
	Query Select(const initializer_list<string>& keys)
	{
		return Query(indexer, keys);
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
	Type* Component(Entity entity, const string& key)
	{
		return indexer->GetComponent<Type>(entity, key);
	}

	/**
     * @brief Sets a hint that helps the memory-manager decide on optimal sizes for its memory blocks.
     * This should be called BEFORE the first instance of component 'key' has been emplaced.
     * There are no guarantees that the hint will be honored.
     * @param key The key for for the requested component.
     * @param size_hint A hint of the maximum number of components with key 'key' to be be expected.
     */
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