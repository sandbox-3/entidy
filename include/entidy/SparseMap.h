#pragma once
#include <memory>
#include <vector>
#include <unordered_map>

namespace entidy
{
using namespace std;

template <typename Type>
class SparseMap
{
	unordered_map<size_t, Type> map;

public:
	void Emplace(size_t key, Type value)
	{
		map.emplace(key, value);
	}

	vector<Type> Select(const vector<size_t>& keys) const
	{
		vector<Type> out;
		for(auto& t : keys)
			out.push_back(map[t]);
		return out;
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