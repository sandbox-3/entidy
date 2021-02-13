#pragma once

#include <memory>
#include <map>
#include <string>
#include <vector>
#include <deque>
#include <iostream>
#include <assert.h>
#include <functional>

namespace entidy
{
    using namespace std;

    template <typename Type>
    class MemoryBlock
    {
    protected:
        std::aligned_storage_t<sizeof(Type), alignof(Type)> *data;
        vector<Type *> pool;
        size_t item_capacity;
        Type *pointer;

        MemoryBlock(size_t item_capacity)
        {
            data = new std::aligned_storage_t<sizeof(Type), alignof(Type)>[item_capacity];
            pointer = reinterpret_cast<Type *>(data);

            for (size_t i = 0; i < item_capacity; i++)
            {
                Type *ptr = pointer + i;
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

        void Push(Type *ptr)
        {
            ptr->~Type();
            pool.push_back(ptr);
        }

        Type *Pop()
        {
            Type *back = pool.back();
            pool.pop_back();
            return back;
        }

        Type *Pointer()
        {
            return pointer;
        }
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
            bool prune = false;
            auto it = pool.begin();
            while (it != pool.end())
            {
                shared_ptr<MemoryBlock<Type>> block = *it;
                Type *range_start = block->Pointer();
                Type *range_end = block->Pointer() + item_capacity;

                if (ptr > range_start && ptr < range_end)
                {
                    block->Push(ptr);
                    ++it;
                    continue;
                }

                if (block->Available() == block->Capacity())
                {
                    if (prune)
                        it = pool.erase(it);
                    prune = true;
                }
                ++it;
            }
        }

        Type *Pop()
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
    };

    class MemoryManager
    {
    protected:
        struct ManagedPool
        {
            shared_ptr<void> pool;
            std::function<void(const string &key, void *ptr)> push;
        };
        unordered_map<string, ManagedPool> pools;

    public:
        MemoryManager()
        {
        }

        template <typename Type>
        void Create(const string &key, size_t block_capacity = 64000)
        {
            assert(pools.find(key) == pools.end());

            size_t size = sizeof(Type);

            auto managed_pool = ManagedPool{};
            managed_pool.pool = shared_ptr<MemoryPool<Type>>(new MemoryPool<Type>(block_capacity));
            managed_pool.push = [&](const string &key, void *ptr) {
                assert(pools.find(key) != pools.end());

                MemoryPool<Type> *mp = static_cast<MemoryPool<Type> *>(pools[key].pool.get());
                mp->Push(static_cast<Type *>(ptr));
            };

            pools.emplace(key, managed_pool);
        }

        void Push(const string &key, void *ptr)
        {
            pools[key].push(key, ptr);
        }

        template <typename Type>
        Type *Pop(const string &key)
        {
            if (pools.find(key) == pools.end())
                Create<Type>(key);

            MemoryPool<Type> *mp = static_cast<MemoryPool<Type> *>(pools[key].pool.get());
            return mp->Pop();
        }

        bool HasKey(const string &key)
        {
            return pools.find(key) != pools.end();
        }
    };
} // namespace entidy