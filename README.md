# Entidy

[![CMake](https://github.com/sandbox-3/entidy/actions/workflows/cmake.yml/badge.svg)](https://github.com/sandbox-3/entidy/actions/workflows/cmake.yml)

`Entidy` is an [Entity-Component-System](https://en.wikipedia.org/wiki/Entity_component_system)
written in C++17.

Features:

1. Separation between C++ types (`struct` or `class`) from `Components`
2. A querying language
3. Automatic memory management
4. Static or Dynamic build.


## Table of Contents

- [Entidy](#entidy)
  - [Table of Contents](#table-of-contents)
  - [Example](#example)
  - [Why Entidy?](#why-entidy)
    - [Code Re-use](#code-re-use)
    - [Dynamic Components](#dynamic-components)
    - [Relationships and Hierarchies](#relationships-and-hierarchies)
  - [Views and Queries](#views-and-queries)
    - [Exact Selection](#exact-selection)
    - [Optional Selection](#optional-selection)
    - [Query Language](#query-language)
    - [Accessing Views without Lambdas](#accessing-views-without-lambdas)
  - [Performance](#performance)
  - [Build](#build)

## Example

```c++
#include <iostream>

#include <entidy/Entidy.h>

using namespace std;
using namespace entidy;

struct Vec3 { int x, y, z; };

int main()
{
  Entidy registry;

  auto e = registry.Create();

  registry.Emplace<Vec3>(e, "position");
  registry.Emplace<Vec3>(e, "velocity");

  auto view = registry.Select({"position", "velocity"})
                      .Having("position & velocity");

  view.Each([&](Entity e, Vec3* pos, Vec3* vel)
  {
      cout << e << "\n";
  });

  return 0;
}
```

For more examples, refer to the `examples` and `benchmark` directories in this
repository.

## Why Entidy?

### Code Re-use

The most evident advantage of separation of C++ types from components is reusing
existing code.

As demonstrated in the example above, the system designer can create a sigle
type representing *similar components* without having to re-write
the types over and over; in our case `Vec3` was used to represent both
`position` and `velocity`, which helps in reducing clutter.

### Dynamic Components

Given that `Entidy` separates C++ types from components by design, it allows
system designers to create components on-the-fly.

```c++
...

auto component_name = GenerateFreshComponentNameSomehow();

registry.Emplace<Vec3>(e, component_name);

...

auto view = registry.Select({component_name})
                    .Having(component_name);

view.Each([&](Entity e, Vec3* x)
{
  cout << e << "\n";
});

...
```

This case is useful for temporary components, such as flags (e.g. mark an
entity for deletion) and tags (e.g. mark an entity as `is_enemy`).

### Relationships and Hierarchies

Let's suppose we want to model an ownership relationship where one entity
own 1 or many other entities.

If we were to have C++ types as components, we would have to model the
relationship inside the types themselves using composition: an owner entity
would need to have a component with a vector of children entities, and children
would need to reference their parent entitie(s) in their respective components.

This approach poses 2 main problems:

1. The overhead of bookkeeping and its maintenance on behalf of the developer,
   which is the product of the redundancy in representing those relationships.
2. At least 2 queries need to be performed in order to produce a view that
   gives us access to the needed components.

`Entidy` solves this elegantly by giving the ability to create components on
the fly to represent those relationships.

```c++
#include <iostream>
#include <string>

#include <entidy/Entidy.h>

using namespace std;

struct City { int id; };

struct Peasant { int id; };

int main()
{
  Entidy registry;

  auto city_1    = registry.Create();
  auto peasant_1 = registry.Create();
  auto peasant_2 = registry.Create();

  registry.Emplace<City>(city_1, "city");
  registry.Emplace<Peasant>(peasant_1, "peasant");
  registry.Emplace<Peasant>(peasant_2, "peasant");

  auto lives_in_city_1 = "lives_in_" + std::to_string(city_1);

  registry.Emplace<Peasant>(peasant_1, lives_in_city_1);
  registry.Emplace<Peasant>(peasant_2, lives_in_city_1);

  auto view = registry.Select({"peasant"})
                      .Having("peasant & " + lives_in_city_1);

  ...

  return 0;
}

```

## Views and Queries

### Exact Selection

In order to select entities having exactly the components you're looking for,
components should figure in `Select` and `Having` as follows:

```c++
auto view = registry.Select({"position", "velocity"})
                    .Having("position & velocity");
```

Additional constaints can be added as such:

```c++
auto view = registry.Select({"position", "velocity"})
                    .Having("position & velocity & (!peasant | city)");
```

### Optional Selection

If you want to query for entities that may or may not have certain componenets,
you can ommit the optional components from `Having` as such:

```c++
auto view = registry.Select({"position", "velocity"})
                    .Having("position");
```

In this case, the view will give you access to `position` and `velocity`, 
however, you have to check whether the `velocity` pointer is `nullptr` before
accessing it.

```c++
view.Each([&](Entity e, Vec3* pos, Vec3* vel)
{
  if(vel != nullptr)
    cout << e << "\n";
});
```

### Query Language

The query language used in `Having` has the following operators:

| Operator | Meaning | Example                |
| -------- | ------- | ---------------------- |
| `&`      | AND     | `position & velocity`  |
| `\|`     | OR      | `position \| velocity`  |
| `!`      | NOT     | `position & !velocity` |

Expressions can be nested using parenthesis as such:

```
position & velocity & !(city | peasant)
```

### Accessing Views without Lambdas

In some cases, the system designer would want to access query results by index,
as opposed to calling the `Each` function and passing it a lambda.

`Entidy` provides the `At` function that works as follows:

```c++
...
auto view = registry.Select({"position", "velocity"})
                    .Having("position & velocity");

for(auto i = 0; i < view.Size(); ++i)
{
  auto entity   = view.At(i); 
  auto position = *view.At<Vec3, 0>(i);
  auto velocity = *view.At<Vec3, 1>(i)
  ...
}
...
```

## Performance

Entidy emphasizes performance when querying components. Our benchmarks (check
the `benchmark` directory) shows that Entidy is comparable in performance to
existing ECS libraries like [entt](https://github.com/skypjack/entt).

## Build

Entidy uses `cmake`. You can specify the following options when building:

| Option                 | Description            | Default |
| ---------------------- | ---------------------- | ------- |
| ENTIDY_BUILD_EXAMPLES  | Build the examples     | ON      |
| ENTIDY_BUILD_BENCHMARK | Build the benchmarks   | ON      |
| ENTIDY_BUILD_STATIC    | Build a static library | ON      |
