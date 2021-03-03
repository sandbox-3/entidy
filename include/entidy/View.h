#pragma once

#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <vector>

#include <entidy/Entidy.h>
#include <entidy/Indexer.h>
#include <entidy/SparseVector.h>

namespace entidy
{
using namespace std;

using Entity = uint32_t;

class IndexerImpl;

class View
{
protected:
	vector<SparseVector<ENTIDY_DEFAULT_SV_SIZE>> data;
	vector<size_t> types;

	View(const vector<SparseVector<ENTIDY_DEFAULT_SV_SIZE>>& data_list, const vector<size_t>& type_list)
		: data(data_list)
		, types(type_list)
	{ }

public:
	template <class Ld>
	struct lambda_type : lambda_type<decltype(&Ld::operator())>
	{ };

	template <class Ret, class Cls, class... Args>
	struct lambda_type<Ret (Cls::*)(Args...) const>
	{
		vector<SparseVector<ENTIDY_DEFAULT_SV_SIZE>> data;
		vector<size_t> types;

		template <size_t Nmax, size_t N, typename Head, typename... Rest>
		constexpr std::tuple<Head, Rest...> GetIndirection(size_t index) const
		{
			if constexpr(N == 1)
				return std::make_tuple(static_cast<Head>((typename std::decay<Head>::type)data[data.size() - N]->Read(index)));

			else if constexpr(N == Nmax)
				return std::tuple_cat(std::make_tuple(static_cast<Head>(data[0]->Read(index))), GetGenerator<Nmax, N - 1, Rest...>(index));

			else
				return std::tuple_cat(std::make_tuple(static_cast<Head>((typename std::decay<Head>::type)data[data.size() - N]->Read(index))), GetGenerator<Nmax, N - 1, Rest...>(index));
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

	/**
     * @brief Iterate over all entities in the view, applying the provided functor on each row.
     * The functor must receive Entity followed by pointers to component types in the order they figure in Entidy::Select.
     * @param fn Any functor or lambda that expects Entity, followed by pointers to selected component types.
     * @throw EntidyException if any of the pointer types in the provided functor does not match with the type associated to the component.
     * @example
     * auto view = entidy.Select({"Position", "Velocity", "Colour"}).Having("Position & Velocity & Colour & Renderable");
     * view.Each([&](Entity e, Vec2f* position, Vec2f* velocity, Color* color){ // ... });
     */
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

	/**
     * @brief Returns the number of entities in this view.
     * @return Number of entities in this view.
     */
	size_t Size() const
	{
		return data[0]->Size();
	}

	/**
     * @brief Returns a pointer to the component at Col for entity at row Row.
     * @tparam Type The component type. 
     * @tparam Col The position of the component in the order set by Entidy::Select.
     * @return A pointer to the requested component.
     * @throw EntidyException if the provided pointer type does not match with the type associated to the component.
     */
	template <typename Type, size_t Col>
	Type* At(size_t row)
	{
		if(typeid(Type*).hash_code() != types[Col + 1])
			throw(EntidyException("Type mismatch for class " + string(typeid(Type).name())));
		return reinterpret_cast<Type*>(data[Col + 1]->Read(row));
	}

	/**
     * @brief Returns the entity at requested row.
     * @return the Entity at selected row.
     */
	Entity At(size_t row)
	{
		return static_cast<Entity>(data[0]->Read(row));
	}

	friend IndexerImpl;
};

} // namespace entidy
