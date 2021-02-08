#pragma once

#include <memory>
#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include <assert.h>

#include <entidy/MemoryManager/MemoryPool.h>

namespace entidy
{
    using namespace std;

    template <typename Type>
    class MemoryPool;

    class MemoryManager
    {
    protected:
        unordered_map<string, shared_ptr<void>> pools;

    public:
        MemoryManager()
        {
        }

        template <typename Type>
        void Create(const string &key, size_t block_capacity = 64000)
        {
            assert(pools.find(key) == pools.end());

            size_t size = sizeof(Type);
            shared_ptr<MemoryPool<Type>> pool = shared_ptr<MemoryPool<Type>>(new MemoryPool<Type>(block_capacity));
            pools.emplace(key, pool);
        }

        template <typename Type>
        void Push(const string &key, Type *ptr)
        {
            assert(pools.find(key) != pools.end());

            MemoryPool<Type> *mp = static_cast<MemoryPool<Type> *>(pools[key].get());
            mp->Push(ptr);
        }

        template <typename Type>
        Type *Pop(const string &key)
        {
            assert(pools.find(key) != pools.end());

            MemoryPool<Type> *mp = static_cast<MemoryPool<Type> *>(pools[key].get());
            return mp->Pop();
        }

        bool HasKey(const string &key)
        {
            return pools.find(key) != pools.end();
        }
    };

} // namespace entidy