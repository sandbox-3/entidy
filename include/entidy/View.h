#pragma once

#include <type_traits>
#include <vector>

#include <entidy/Registry.h>
#include <entidy/SparseVector.h>
#include <entidy/Indexer.h>

namespace entidy
{
using namespace std;

using Entity = uint32_t;

class IndexerImpl;

class View
{
protected:
	vector<SparseVector<DEFAULT_SV_SIZE>> data;

	View(const vector<SparseVector<DEFAULT_SV_SIZE>>& data)
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
		vector<SparseVector<DEFAULT_SV_SIZE>> data;

		template <size_t Nmax, size_t N, typename Head, typename... Rest>
		constexpr std::tuple<Head, Rest...> Indirection(size_t index) const
		{
			if constexpr(N == 1)
				return std::make_tuple(static_cast<Head>((typename std::decay<Head>::type)data[data.size() - N]->Read(index)));

            else if constexpr (N == Nmax)
				return std::tuple_cat(std::make_tuple(static_cast<Head>(data[0]->Read(index))),
					Generator<Nmax, N - 1, Rest...>(index));
                    
			else
				return std::tuple_cat(std::make_tuple(static_cast<Head>((typename std::decay<Head>::type)data[data.size() - N]->Read(index))),
					Generator<Nmax, N - 1, Rest...>(index));
		}

		template <size_t Nmax, size_t N, typename... T>
		constexpr std::tuple<T...> Generator(size_t index) const
		{
			return Indirection<Nmax, N, T...>(index);
		}

		constexpr tuple<Args...> Get(size_t index)
		{
			return Generator<sizeof...(Args), sizeof...(Args), Args...>(index);
		}
	};

	template <typename F>
	void Each(F&& fn) const
	{
		lambda_type<F> lt;
		lt.data = data;
        
		for(size_t index = 0; index < data[0]->Size(); index++)
		{
			std::apply(fn, lt.Get(index));
		}
	}

    size_t Size() const
    {
        return data[0]->Size();
    }

	template <typename Type>
    Type* At(size_t row, size_t col)
    {
        return static_cast<Type*>(data[col+1]->Read(row));
    }

    Entity At(size_t row)
    {
        return static_cast<Entity>(data[0]->Read(row));
    }

	friend IndexerImpl;
};

} // namespace entidy