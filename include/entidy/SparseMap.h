#pragma once
#include <memory>
#include <vector>
#include <unordered_map>

#include <entidy/PagedVector.h>
#include <entidy/MemoryManager.h>

namespace entidy
{
using namespace std;

template <typename Type>
class SparseMap
{
	unordered_map<size_t, Type> map;

public:
    SparseMap()
    {
    }

	void Emplace(size_t key, Type value)
	{
		map.emplace(key, value);
	}

    template <typename IterT>
	void Select(PagedVector<1024> &out, const IterT& begin, const IterT& end) const
	{
        size_t i = 0;
        auto it = begin;
		while(it != end)
        {
			out.Write(i++, map.find(*it)->second);
            ++it;
        }
	}

	Type Select(size_t key) const
	{
		return map.find(key)->second;
	}

	void Remove(size_t key)
	{
		map.erase(key);
	}
};

} // namespace entidy