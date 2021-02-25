#define CATCH_CONFIG_MAIN

#include <algorithm>
#include <iostream>

#include "catch2/catch.hpp"
#include "entidy/Registry.h"
#include "entt.hpp"

template <size_t Size>
struct Component
{
	char data[Size];
};

constexpr size_t word_size = sizeof(intptr_t);
constexpr size_t dword_size = 2 * word_size;

std::vector<entidy::Entity> entidy_vector_of_n_entities(size_t size)
{
	std::vector<entidy::Entity> entities(size);
	std::uint32_t n = 1;
	std::generate(entities.begin(), entities.end(), [&n]() mutable { return n++; });

	return entities;
}

std::vector<entt::entity> entt_vector_of_n_entities(size_t size)
{
	return std::vector<entt::entity>(size);
}

TEST_CASE("Creating 100000 entities")
{
	BENCHMARK_ADVANCED("ENTT")(Catch::Benchmark::Chronometer meter)
	{
		entt::registry registry;

		meter.measure([&]() {
			for(auto i = 0; i < 1000000L; i++)
			{
				registry.create();
			}
		});
	};

	BENCHMARK_ADVANCED("entidy")(Catch::Benchmark::Chronometer meter)
	{
		auto registry = entidy::RegistryFactory::New();

		meter.measure([&]() {
			for(auto i = 0; i < 1000000L; i++)
			{
				registry->Create();
			}
		});
	};
}

TEST_CASE("Creating 100000 entities Bulk")
{
	BENCHMARK_ADVANCED("ENTT")(Catch::Benchmark::Chronometer meter)
	{
		entt::registry registry;
		auto entities = entt_vector_of_n_entities(100000);

		meter.measure([&]() { registry.create(entities.begin(), entities.end()); });
	};

	BENCHMARK_ADVANCED("entidy")(Catch::Benchmark::Chronometer meter)
	{
		auto registry = entidy::RegistryFactory::New();

		auto entities = entidy_vector_of_n_entities(100000);

		meter.measure([&]() {
			for(auto i = 0; i < 1000000L; i++)
			{
				registry->Create();
			}
		});
	};
}

TEST_CASE("Creating 100000 entities + Emplace Components")
{
	BENCHMARK_ADVANCED("ENTT")
	(Catch::Benchmark::Chronometer meter)
	{
		entt::registry registry;
		auto entities = entt_vector_of_n_entities(100000);

		registry.create(entities.begin(), entities.end());

		meter.measure([&]() {
			for(const auto entity : entities)
			{
				registry.emplace<Component<3 * word_size>>(entity);
				registry.emplace<Component<8 * word_size>>(entity);
			}
		});
	};

	BENCHMARK_ADVANCED("entidy")(Catch::Benchmark::Chronometer meter)
	{
		auto registry = entidy::RegistryFactory::New();

		auto entities = entidy_vector_of_n_entities(100000);

		for(auto i = 0; i < entities.size(); i++)
		{
			registry->Create();
		}

		meter.measure([&]() {
			for(auto entity : entities)
			{
				registry->Emplace(entity, "Comp003xWord", Component<3 * word_size>{});
				registry->Emplace(entity, "Comp008xWord", Component<8 * word_size>{});
			}
		});
	};
}

TEST_CASE("Creating 100000 entities + Emplace Bulk Components")
{
	BENCHMARK_ADVANCED("ENTT")
	(Catch::Benchmark::Chronometer meter)
	{
		entt::registry registry;
		auto entities = entt_vector_of_n_entities(100000);

		registry.create(entities.begin(), entities.end());

		meter.measure([&]() {
			registry.insert<Component<3 * word_size>>(entities.begin(), entities.end());
			registry.insert<Component<8 * word_size>>(entities.begin(), entities.end());
		});
	};

	BENCHMARK_ADVANCED("entidy")(Catch::Benchmark::Chronometer meter)
	{
		auto registry = entidy::RegistryFactory::New();

		auto entities = entidy_vector_of_n_entities(100000);

		for(auto i = 0; i < entities.size(); i++)
		{
			registry->Create();
		}

		meter.measure([&]() {
			for(const auto entity : entities)
			{
				registry->Emplace(entity, "Comp003xWord", Component<3 * word_size>{});
				registry->Emplace(entity, "Comp008xWord", Component<8 * word_size>{});
			}
		});
	};
}

TEST_CASE("Removing 100000 components from their entities")
{
	BENCHMARK_ADVANCED("ENTT")
	(Catch::Benchmark::Chronometer meter)
	{
		entt::registry registry;
		auto entities = entt_vector_of_n_entities(100000);

		registry.create(entities.begin(), entities.end());
		registry.insert<int>(entities.begin(), entities.end());

		meter.measure([&]() {
			for(auto entity : registry.view<int>())
			{
				registry.remove<int>(entity);
			}
		});
	};

	BENCHMARK_ADVANCED("entidy")(Catch::Benchmark::Chronometer meter)
	{

		meter.measure([]() {
			auto registry = entidy::RegistryFactory::New();
			auto entities = entidy_vector_of_n_entities(100000);

			for(auto i = 0; i < entities.size(); i++)
			{
				registry->Create();
			}

			for(const auto entity : entities)
			{
				registry->Emplace(entity, "CompInt", 0);
			}
			auto selector = registry->Select({"CompInt"});
			selector.Having("CompInt").Each([&registry](entidy::Entity e, int x) { registry->Erase(e, "CompInt"); });
		});
	};
}

TEST_CASE("Removing 99999 components from their entities at once")
{
	BENCHMARK_ADVANCED("ENTT")
	(Catch::Benchmark::Chronometer meter)
	{
		entt::registry registry;
		auto entities = entt_vector_of_n_entities(100000);

		registry.create(entities.begin(), entities.end());
		registry.insert<int>(entities.begin(), entities.end());

		meter.measure([&]() {
			auto view = registry.view<int>();
			registry.remove<int>(++view.begin(), view.end());
		});
	};
}

TEST_CASE("Removing 100000 components from their entities at once")
{
	BENCHMARK_ADVANCED("ENTT")
	(Catch::Benchmark::Chronometer meter)
	{
		entt::registry registry;
		auto entities = entt_vector_of_n_entities(100000);

		registry.create(entities.begin(), entities.end());
		registry.insert<int>(entities.begin(), entities.end());

		meter.measure([&]() {
			auto view = registry.view<int>();
			registry.remove<int>(view.begin(), view.end());
		});
	};

	BENCHMARK_ADVANCED("entidy")
	(Catch::Benchmark::Chronometer meter)
	{
		auto registry = entidy::RegistryFactory::New();
		auto entities = entidy_vector_of_n_entities(100000);

		for(auto i = 0; i < entities.size(); i++)
		{
			registry->Create();
		}

		for(const auto entity : entities)
		{
			registry->Emplace(entity, "CompInt", 0);
		}

		auto selector = registry->Select({"CompInt"});

		meter.measure([&]() { selector.Having("CompInt").Each([&registry](entidy::Entity e, int* x) { registry->Erase(e, "CompInt"); }); });
	};
}

TEST_CASE("Destorying 100000 entities")
{
	BENCHMARK_ADVANCED("ENTT")
	(Catch::Benchmark::Chronometer meter)
	{
		entt::registry registry;
		auto entities = entt_vector_of_n_entities(100000);

		registry.create(entities.begin(), entities.end());
		registry.insert<int>(entities.begin(), entities.end());

		meter.measure([&]() {
			for(auto entity : registry.view<int>())
			{
				registry.destroy(entity);
			}
		});
	};
}

TEST_CASE("Destorying 100000 entities Bulk")
{

	BENCHMARK_ADVANCED("ENTT")
	(Catch::Benchmark::Chronometer meter)
	{
		entt::registry registry;
		std::vector<entt::entity> entities(100000);

		registry.create(entities.begin(), entities.end());
		registry.insert<int>(entities.begin(), entities.end());

		meter.measure([&]() {
			auto view = registry.view<int>();
			registry.destroy(view.begin(), view.end());
		});
	};
}
