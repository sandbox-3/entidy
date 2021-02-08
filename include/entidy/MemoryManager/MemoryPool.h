#pragma once

#include <memory>
#include <map>
#include <string>
#include <vector>
#include <deque>
#include <iostream>

namespace entidy
{
    using namespace std;

    class MemoryManager;

    template <typename Type>
    class MemoryPool;

    template <typename Type>
    class MemoryBlock
    {
    protected:
        std::aligned_storage_t<sizeof(Type), alignof(Type)> * data;
        deque <Type*> pool;
        size_t item_capacity;
        Type * pointer;

        MemoryBlock(size_t item_capacity)
        {
            data = new std::aligned_storage_t<sizeof(Type), alignof(Type)>[item_capacity];
            pointer = reinterpret_cast<Type*>(data);

            for (size_t i = 0; i < item_capacity; i++)
            {
                Type * ptr = pointer + i;
                pool.push_back(ptr);
            }
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

        void Push(Type *ptr)
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

        friend class MemoryPool<Type>;
    };

    template <typename Type>
    class MemoryPool
    {
    protected:
        deque<shared_ptr<MemoryBlock<Type>>> pool;
        size_t item_capacity;

        MemoryPool(size_t block_size)
        {
            this->item_capacity = max((block_size / sizeof(Type)), size_t(2));
        }

    public:

        ~MemoryPool()
        {
        }

        void Push(Type *ptr)
        {
            auto it = pool.begin();
            while (it != pool.end())
            {
                shared_ptr<MemoryBlock<Type>> block = *it;
                Type* range_start = block->Pointer();
                Type* range_end = block->Pointer() + item_capacity;

                if (ptr > range_start && ptr < range_end)
                {
                    block->Push(ptr);
                    ++it;
                    continue;
                }

                if (block->Available() == block->Capacity())
                    it = pool.erase(it);
                ++it;
            }
        }

        Type* Pop()
        {
            for (auto &block : pool)
            {
                if (block->Available() > 0)
                    return block->Pop();
            }

            shared_ptr<MemoryBlock<Type>> new_block = shared_ptr<MemoryBlock<Type>>(new MemoryBlock<Type>(item_capacity));
            pool.push_back(new_block);
            return new_block->Pop();
        }

        friend MemoryManager;
    };

} // namespace entidy