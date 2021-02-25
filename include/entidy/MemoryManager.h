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
class MemoryPoolImpl;

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
		delete []data;
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

    friend MemoryPoolImpl<Type>;
};

class MemoryManagerImpl;

template <typename Type>
class MemoryPoolImpl
{
protected:
	vector<MemoryBlock<Type>*> blocks;
	size_t item_capacity;

	MemoryPoolImpl(size_t item_capacity)
	{
		this->item_capacity = item_capacity;
	}

public:
	~MemoryPoolImpl()
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
                if(block->Available() == block->Capacity() && blocks.size() > 1)
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
	shared_ptr<void> pool;
	std::function<void(MemoryManagerImpl * sender, intptr_t ptr)> push;
	size_t counter;
 
public:       
	template <typename Type>
	static shared_ptr<MemoryManagerImpl> Create(size_t size_hint = 0)
	{
		size_t maxc = size_t(4 * 1024 * 1024) / sizeof(Type);
		size_t defc = size_hint == 0 ? size_t(2 * 1024 * 1024) / sizeof(Type) : size_hint;

		size_t block_capacity = max(size_t(1), min(defc, maxc));

		shared_ptr<MemoryManagerImpl> managed_pool(new MemoryManagerImpl());
		managed_pool->pool = shared_ptr<MemoryPoolImpl<Type>>(new MemoryPoolImpl<Type>(block_capacity));
		managed_pool->push = [&](MemoryManagerImpl * sender, intptr_t ptr) {
			MemoryPoolImpl<Type>* mp = static_cast<MemoryPoolImpl<Type>*>(sender->pool.get());
			mp->Push(ptr);
		};
        return managed_pool;
	}

	~MemoryManagerImpl()
	{
	}

	void Push(intptr_t ptr)
	{
		push(this, ptr);
		counter--;
	}

	template <typename Type>
	Type* Pop()
	{
		counter++;

		MemoryPoolImpl<Type>* mempool = static_cast<MemoryPoolImpl<Type>*>(pool.get());
		return mempool->Pop();
	}
};
} // namespace entidy