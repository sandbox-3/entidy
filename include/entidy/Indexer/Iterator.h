#pragma once

#include <type_traits>

#include <entidy/Entity.h>

namespace entidy
{

    using namespace std;

    class Iterator
    {
    protected:
        vector<Entity> entities;
        intptr_t *data;
        size_t index;
        size_t rows;
        size_t cols;

    public:
    
        template <class Ld>
        struct lambda_type
            : lambda_type<decltype(&Ld::operator())>
        {
        };

        template <class Ret, class Cls, class... Args>
        struct lambda_type<Ret (Cls::*)(Args...) const>
        {
            vector<Entity> entities;
            intptr_t *data;
            size_t index;
            size_t rows;
            size_t cols;

            template <size_t N, typename Head, typename... Rest>
            constexpr std::tuple<Head, Rest...> Indirection(size_t index) const
            {
                if constexpr (N == 1)
                    return std::make_tuple(static_cast<Head>(*(typename std::decay<Head>::type *)data[index * rows + cols]));
                else
                    return std::tuple_cat(std::make_tuple(static_cast<Head>(*(typename std::decay<Head>::type *)data[index * rows + cols])), Generator<N - 1, Rest...>(index));
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
        void Each(F &&fn) const
        {
            lambda_type<F> lt;

            auto index = 0;
            for (index = 0; index < rows; index++)
            {
                std::apply(fn, lt.Get(index));
            }
        }

        // template <typename Func, typename... Args>
        // static auto getFuncType(Func* func = nullptr, Args args...) -> decltype((*func)(args...));

        // template <typename Function, typename... Args>
        // void Each(Function && lambda) const
        // {
        //     Each<decltype(getFuncType<Function, Args...>())>(lambda);
        // }
    };

} // namespace entidy