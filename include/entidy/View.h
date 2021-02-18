#pragma once

#include <type_traits>
#include <vector>

#include <entidy/PagedVector.h>
#include <entidy/Indexer.h>

namespace entidy
{
using namespace std;
class IndexerImpl;

class View
{
protected:
	vector<PagedVector<1024>> data;

	View(const vector<PagedVector<1024>>& data)
	{
		this->data = data;
	}

public:
	template <class Ld>
	struct lambda_type : lambda_type<decltype(&Ld::operator())>
	{ };

	template <class Ret, class Cls, class... Args>
	struct lambda_type<Ret (Cls::*)(Args...) const>
	{
		vector<PagedVector<1024>> data;

		template <size_t N, typename Head, typename... Rest>
		constexpr std::tuple<Head, Rest...> Indirection(size_t index) const
		{
			if constexpr(N == 1)
				return std::make_tuple(static_cast<Head>(
					*(typename std::decay<Head>::type*)data[data.size() - N].Read(index)));
			else
				return std::tuple_cat(
					std::make_tuple(static_cast<Head>(
						*(typename std::decay<Head>::type*)data[data.size() - N].Read(index))),
					Generator<N - 1, Rest...>(index));
		}

		template <size_t N, typename... T>
		constexpr std::tuple<T...> Generator(size_t index) const
		{
			return Indirection<N, T...>(index);
		}

		constexpr tuple<Args...> Get(size_t index)
		{
			return Generator<sizeof...(Args), Args...>(index);
		}
	};

	template <typename F>
	void Each(F&& fn) const
	{
		lambda_type<F> lt;
		lt.data = data;

		for(size_t index = 0; index < data[0].Size(); index++)
		{
			std::apply(fn, lt.Get(index));
		}
	}

	friend IndexerImpl;
};

} // namespace entidy