#pragma once

#include <type_traits>
#include <vector>
#include <typeinfo>
#include<iostream>

#include <entidy/Entidy.h>
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
    vector<size_t> types;

	View(const vector<SparseVector<DEFAULT_SV_SIZE>>& data, const vector<size_t> &types)
	{
		this->data = data;
        this->types = types;
	}

public:
	template <class Ld>
	struct lambda_type : lambda_type<decltype(&Ld::operator())>
	{ };

	template <class Ret, class Cls, class... Args>
	struct lambda_type<Ret (Cls::*)(Args...) const>
	{
		vector<SparseVector<DEFAULT_SV_SIZE>> data;
        vector<size_t> types;

		template <size_t Nmax, size_t N, typename Head, typename... Rest>
		constexpr std::tuple<Head, Rest...> GetIndirection(size_t index) const
		{
			if constexpr(N == 1)
				return std::make_tuple(static_cast<Head>((typename std::decay<Head>::type)data[data.size() - N]->Read(index)));

            else if constexpr (N == Nmax)
				return std::tuple_cat(std::make_tuple(static_cast<Head>(data[0]->Read(index))),
					GetGenerator<Nmax, N - 1, Rest...>(index));
                    
			else
				return std::tuple_cat(std::make_tuple(static_cast<Head>((typename std::decay<Head>::type)data[data.size() - N]->Read(index))),
					GetGenerator<Nmax, N - 1, Rest...>(index));
		}

		template <size_t Nmax, size_t N, typename... T>
		constexpr std::tuple<T...> GetGenerator(size_t index) const
		{
			return GetIndirection<Nmax, N, T...>(index);
		}

		constexpr tuple<Args...> Get(size_t index)
		{
			return GetGenerator<sizeof...(Args), sizeof...(Args), Args...>(index);
		}

		template <size_t N, typename Head, typename... Rest>
		constexpr void TypeCheckIndirection() const
		{
			if constexpr(N == 1)
            {
                if(types[types.size() - N] != 0 && typeid(Head).hash_code() != types[types.size() - N])
                    throw(EntidyException("Type mismatch for class " + string(typeid(Head).name())));
            }
			else
            {
                if(types[types.size() - N] != 0 && typeid(Head).hash_code() != types[types.size() - N])
                    throw(EntidyException("Type mismatch for class " + string(typeid(Head).name())));
				TypeCheckGenerator<N - 1, Rest...>();
            }
		}

		template <size_t N, typename... T>
		constexpr void TypeCheckGenerator() const
		{
			TypeCheckIndirection<N, T...>();
		}

		constexpr void TypeCheck()
		{
			TypeCheckGenerator<sizeof...(Args), Args...>();
		}
	};


	template <typename F>
	void Each(F&& fn) const
	{
        if(data[0]->Size() == 0)
            return;

		lambda_type<F> lt;
		lt.data = data;
		lt.types = types;
        
        lt.TypeCheck();

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
        if(typeid(Type*).hash_code() != types[col+1])
            throw(EntidyException("Type mismatch for class " + string(typeid(Type).name())));
        return reinterpret_cast<Type*>(data[col+1]->Read(row));
    }

    Entity At(size_t row)
    {
        return static_cast<Entity>(data[0]->Read(row));
    }

	friend IndexerImpl;
};

} // namespace entidy