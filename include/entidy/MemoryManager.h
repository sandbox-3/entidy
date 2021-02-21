#pragma once

#include <assert.h>
#include <functional>
#include <unordered_map>
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
	intptr_t start;
	intptr_t end;

	MemoryBlock(size_t item_capacity)
	{
		this->item_capacity = item_capacity;
        pool.resize(item_capacity);
		data = new std::aligned_storage_t<sizeof(Type), alignof(Type)>[item_capacity];

		Type* pointer = reinterpret_cast<Type*>(data);
		for(size_t i = 0; i < item_capacity; i++)
		{
			Type* ptr = pointer + i;
			pool[i] = ptr;
		}
        start = (intptr_t)pool[0];
        end = (intptr_t)pool.back();
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

	intptr_t PointerStart()
	{
		return start;
	}

	intptr_t PointerEnd()
	{
		return end;
	}

	friend MemoryPool<Type>;
};

class MemoryManagerImpl;

template <typename Type>
class MemoryPool
{
protected:
	vector<MemoryBlock<Type>*> blocks;
	size_t item_capacity;

	MemoryPool(size_t item_capacity)
	{
		this->item_capacity = item_capacity;
	}

public:
	~MemoryPool()
	{
		for(MemoryBlock<Type>* it : blocks)
			delete it;
	}

	void Push(intptr_t ptr)
	{
		for(size_t i = 0; i < blocks.size(); i++)
		{
			MemoryBlock<Type>* block = blocks[i];
			intptr_t range_start = block->PointerStart();
			intptr_t range_end = block->PointerEnd();

			if(ptr >= range_start && ptr <= range_end)
			{
				block->Push((Type*)ptr);
                if(block->Available() == block->Capacity())
                {
                    delete block;
                    blocks[i] = blocks.back();
                    blocks.pop_back();
                }
				return;
			}
		}
	}

	Type* Pop()
	{
		for(auto& block : blocks)
		{
			if(block->Available() > 0)
				return block->Pop();
		}

		MemoryBlock<Type>* new_block = new MemoryBlock<Type>(item_capacity);
		blocks.push_back(new_block);
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
	unordered_map<string, ManagedPool*> pools;
	unordered_map<string, size_t> hints;

	template <typename Type>
	void Create(const string& key, size_t size_hint)
	{
		size_t maxc = size_t(2048576) / sizeof(Type);
		size_t defc = size_hint == 0 ? size_t(2048576) / sizeof(Type) : size_hint;

		size_t block_capacity = max(size_t(1), min(defc, maxc));

		ManagedPool* managed_pool = new ManagedPool();
		managed_pool->pool = shared_ptr<MemoryPool<Type>>(new MemoryPool<Type>(block_capacity));
		managed_pool->push = [&](const string& key, intptr_t ptr) {
			MemoryPool<Type>* mp = static_cast<MemoryPool<Type>*>(pools[key]->pool.get());
			mp->Push(ptr);
		};

		pools.emplace(key, managed_pool);
	}

public:
	~MemoryManagerImpl()
	{
		for(auto& it : pools)
			delete it.second;
	}

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
		{
			if(it->second->counter == 0)
			{
				delete it->second;
				it = pools.erase(it);
			}
		}
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