#pragma once

#include <assert.h>
#include <deque>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace entidy
{
using namespace std;

template <typename Type>
class MemoryPool;

template <typename Type>
class MemoryBlock
{
protected:
	std::aligned_storage_t<sizeof(Type), alignof(Type)>* data;
	vector<Type*> pool;
	size_t item_capacity;
	Type* pointer;

	MemoryBlock(size_t item_capacity)
	{
		data = new std::aligned_storage_t<sizeof(Type), alignof(Type)>[item_capacity];
		pointer = reinterpret_cast<Type*>(data);

		for(size_t i = 0; i < item_capacity; i++)
		{
			Type* ptr = pointer + i;
			pool.push_back(ptr);
		}
	}

public:
	~MemoryBlock()
	{
		delete[] data;
	}

	size_t Capacity()
	{
		return item_capacity;
	}

	size_t Available()
	{
		return pool.size();
	}

	void Push(Type* ptr)
	{
		ptr->~Type();
		pool.push_back(ptr);
	}

	Type* Pop()
	{
		Type* back = pool.back();
		pool.pop_back();
		return back;
	}

	Type* Pointer()
	{
		return pointer;
	}

	friend MemoryPool<Type>;
};

class MemoryManagerImpl;

template <typename Type>
class MemoryPool
{
protected:
	deque<shared_ptr<MemoryBlock<Type>>> pool;
	size_t item_capacity;

	MemoryPool(size_t item_capacity)
	{
		this->item_capacity = item_capacity;
	}

public:
	~MemoryPool() { }

	void Push(Type* ptr)
	{
		bool prune = false;
		auto it = pool.begin();
		while(it != pool.end())
		{
			shared_ptr<MemoryBlock<Type>> block = *it;
			Type* range_start = block->Pointer();
			Type* range_end = block->Pointer() + item_capacity;

			if(ptr > range_start && ptr < range_end)
			{
				block->Push(ptr);
				++it;
				continue;
			}

			if(block->Available() == block->Capacity())
			{
				if(prune)
					it = pool.erase(it);
				prune = true;
			}
			++it;
		}
	}

	Type* Pop()
	{
		for(auto& block : pool)
		{
			if(block->Available() > 0)
				return block->Pop();
		}

		shared_ptr<MemoryBlock<Type>> new_block =
			shared_ptr<MemoryBlock<Type>>(new MemoryBlock<Type>(item_capacity));
		pool.push_back(new_block);
		return new_block->Pop();
	}

	friend MemoryManagerImpl;
};

class MemoryManagerImpl;
using MemoryManager = shared_ptr<MemoryManagerImpl>;

class MemoryManagerImpl
{
protected:
	struct ManagedPool
	{
		shared_ptr<void> pool;
		std::function<void(const string& key, intptr_t ptr)> push;
		size_t counter;
	};
	unordered_map<string, shared_ptr<ManagedPool>> pools;
	unordered_map<string, size_t> hints;

	template <typename Type>
	void Create(const string& key, size_t size_hint)
	{
		assert(pools.find(key) == pools.end());

		size_t maxc = size_t(65536) / sizeof(Type);
		size_t defc = size_hint == 0 ? size_t(32768) / sizeof(Type) : size_hint;

		size_t block_capacity = max(size_t(1), min(defc, maxc));

		auto managed_pool = make_shared<ManagedPool>();
		managed_pool->pool = shared_ptr<MemoryPool<Type>>(new MemoryPool<Type>(block_capacity));
		managed_pool->push = [&](const string& key, intptr_t ptr) {
			assert(pools.find(key) != pools.end());
			MemoryPool<Type>* mp = static_cast<MemoryPool<Type>*>(pools[key]->pool.get());
			mp->Push((Type*)(ptr));
		};

		pools.emplace(key, managed_pool);
	}

public:
	MemoryManagerImpl() { }

	void Push(const string& key, intptr_t ptr)
	{
		auto it = pools.find(key);
		assert(it != pools.end());

		it->second->push(key, ptr);
		it->second->counter--;
	}

	template <typename Type>
	Type* Pop(const string& key)
	{
		auto it = pools.find(key);
		if(it == pools.end())
			Create<Type>(key, hints[key]);

		it = pools.find(key);

		it->second->counter++;
		MemoryPool<Type>* mp = static_cast<MemoryPool<Type>*>(it->second->pool.get());
		return mp->Pop();
	}

	void CleanUp()
	{
		auto it = pools.begin();
		while(it != pools.end())
			if(it->second->counter == 0)
				it = pools.erase(it);
	}

	void SizeHint(const string& key, size_t hint)
	{
		hints.emplace(key, hint);
	}

	bool HasKey(const string& key)
	{
		return pools.find(key) != pools.end();
	}
};
} // namespace entidy