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

	/**
     * @brief Creates a block and pre-allocates elements of given Type in cache-aligned and contiguous storage.
     * @param item_capacity The number of items to be allocated in the block.
     */
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
	/**
     * @brief Deallocates all the data in the block.
     */
	~MemoryBlock()
	{
		delete []data;
	}

	/**
     * @brief Returns the number of items that the block can hold.
     * @return The block capacity.
     */
	size_t Capacity()
	{
		return item_capacity;
	}

	/**
     * @brief Returns the number of items currently available in the block.
     * @return The available count.
     */
	size_t Available()
	{
		return pool.size();
	}

	/**
     * @brief Pushes an item to the back of the block.
     * @param ptr A pointer to the discarded item.
     */
	void Push(Type* ptr)
	{
		ptr->~Type();
		pool.push_back(ptr);
	}

	/**
     * @brief Returns and removes an item from the back of the block.
     * @return A pointer to the requested item.
     */
	Type* Pop()
	{
		Type* back = pool.back();
		pool.pop_back();
		return back;
	}

	/**
     * @brief Returns a pointer to the beginning of the block.
     * @return A pointer to the beginning of the block.
     */
	intptr_t PointerStart()
	{
		return start;
	}

	/**
     * @brief Returns a pointer to the end of the block.
     * @return A pointer to the end of the block.
     */
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
	/**
     * @brief De-allocates all the blocks in the pool.
     */
	~MemoryPoolImpl()
	{
		for(MemoryBlock<Type>* it : blocks)
			delete it;
	}

	/**
     * @brief Returns an item to the block from which it came.
     * If, after the new item is added the block becomes completely unused, it is de-allocated.
     * @param ptr A pointer to the object being discarded.
     */
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

	/**
     * @brief Returns an item from the first non-empty block managed by the pool.
     * If no empty blocks are found, a new block is created.
     * @return A pointer to the requested object.
     */
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

	/**
     * @brief Creates a new pool that managed memory blocks of objects of a given Type.
     * @tparam Type of the requested object pool.
     * @param size_hint Optional hint to set the number of items per block.
     * @return A memory manager object that manages the pool of objects of type Type.
     */
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

	/**
     * @brief Returns a single item back to the pool.
     * If a block is unused, it is de-allocated.
     * @param ptr A pointer to the object to be discarded.
     */
	void Push(intptr_t ptr)
	{
		push(this, ptr);
		counter--;
	}

	/**
     * @brief Pops a single item from the pool.
     * If all blocks are used, creates a new block.
     * @tparam Type of the requested object.
     * @return The instance of the object popped from the pool.
     */
	template <typename Type>
	Type* Pop()
	{
		counter++;

		MemoryPoolImpl<Type>* mempool = static_cast<MemoryPoolImpl<Type>*>(pool.get());
		return mempool->Pop();
	}
};
} // namespace entidy