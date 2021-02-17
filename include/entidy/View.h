#pragma once

#include <type_traits>
#include <vector>

#include <entidy/Indexer.h>

namespace entidy
{
using namespace std;
class IndexerImpl;

class View
{
protected:
	vector<intptr_t> data;
	size_t rows;
	size_t cols;

	View(const vector<intptr_t>& data, size_t rows, size_t cols)
	{
		this->data = data;
		this->rows = rows;
		this->cols = cols;
	}

public:
	template <class Ld>
	struct lambda_type : lambda_type<decltype(&Ld::operator())>
	{ };

	template <class Ret, class Cls, class... Args>
	struct lambda_type<Ret (Cls::*)(Args...) const>
	{
		vector<intptr_t> data;
		size_t rows;
		size_t cols;

		template <size_t N, typename Head, typename... Rest>
		constexpr std::tuple<Head, Rest...> Indirection(size_t index) const
		{
			if constexpr(N == 1)
				return std::make_tuple(static_cast<Head>(
					*(typename std::decay<Head>::type*)data[index * rows + (cols - N - 1)]));
			else
				return std::tuple_cat(
					std::make_tuple(static_cast<Head>(
						*(typename std::decay<Head>::type*)data[index * rows + (cols - N - 1)])),
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
		lt.data = &data[0];
		lt.rows = rows;
		lt.cols = cols;

		for(size_t index = 0; index < rows; index++)
		{
			std::apply(fn, lt.Get(index));
		}
	}

	friend IndexerImpl;
};

} // namespace entidy