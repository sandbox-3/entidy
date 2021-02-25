#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <thread>

#include "entidy/Entidy.h"
#include "entt.hpp"

using namespace std::chrono_literals;

struct timer final
{
	timer()
		: start{std::chrono::system_clock::now()}
	{ }

	void elapsed()
	{
		auto now = std::chrono::system_clock::now();
		std::cout << std::chrono::duration<double>(now - start).count() << " seconds" << std::endl;
	}

private:
	std::chrono::time_point<std::chrono::system_clock> start;
};

template <size_t T>
struct Comp
{
	char a[T * 64];
};

class BenchmarkTarget
{
public:
	virtual void Scenario1(unsigned int seed) = 0;
	virtual void Scenario2(unsigned int seed) = 0;
};

template <typename T>
class UniformRandom
{
public:
	UniformRandom(long unsigned int seed = 42)
		: eng{seed}
		, urd{0, 1}
	{
		// If you don't want to set the seed:
		// --
		// std::random_device r;
		// std::seed_seq ssq{r()};
		// --
		// And pass it to the engine
	}

	bool operator()(T p)
	{
		return urd(eng) <= p;
	}

private:
	std::default_random_engine eng;
	std::uniform_real_distribution<T> urd;
};

namespace entidy_benchmark
{
using namespace entidy;
using namespace entt;

class Entidy : public BenchmarkTarget
{
	size_t count;

public:
	Entidy(size_t count)
	{
		this->count = count;
	}

	virtual void Scenario1(unsigned int seed) override
	{
		auto proba = UniformRandom<float>{seed};
		auto registry = RegistryFactory::New();
		auto t0 = timer{};

		for(size_t i = 0; i < count; i++)
		{
			auto e = registry->Create();
			registry->Emplace<Comp<1>>(e, "Comp1");
		}
		t0.elapsed();
		timer t1;
		{
			auto query = registry->Select({"Comp1"});
			auto it = query.Having("Comp1");
			it.Each([&](Entity e, Comp<1>* comp1) {
				comp1->a[0] = 1;
				if(proba(0.25))
					registry->Emplace<Comp<2>>(e, "Comp2");
				if(proba(0.25))
					registry->Emplace<Comp<3>>(e, "Comp3");
				if(proba(0.25))
					registry->Emplace<Comp<4>>(e, "Comp4");
				if(proba(0.25))
					registry->Emplace<Comp<5>>(e, "Comp5");
			});
		}
		t1.elapsed();
		timer t2;
		{
			auto query = registry->Select({"Comp2", "Comp3"});
			auto it = query.Having("Comp2 & Comp3");
			it.Each([&](Entity e, Comp<2>* comp2, Comp<3>* comp3) {
				comp2->a[0] = 1;
				comp3->a[0] = 1;
				if(proba(0.25))
					registry->Erase(e, "Comp2");
				if(proba(0.25))
					registry->Erase(e, "Comp3");
			});
		}
		t2.elapsed();
		timer t3;
		{
			auto query = registry->Select({"Comp4", "Comp5"});
			auto it = query.Having("Comp4 & Comp5");
			it.Each([&](Entity e, Comp<4>* comp4, Comp<5>* comp5) {
				comp4->a[0] = 1;
				comp5->a[0] = 1;
				if(proba(0.25))
					registry->Erase(e);
			});
		}
		t3.elapsed();
		timer t4;
		{
			auto query = registry->Select({"Comp1"});
			auto it = query.Having("Comp1");
			it.Each([&](Entity e, Comp<1>* comp1) {
				comp1->a[0] = 1;
				//if(RandProb(0.25))
				registry->Erase(e);
			});
		}
		t4.elapsed();
	}

	virtual void Scenario2(unsigned int seed) override
	{
		auto proba = UniformRandom<float>{seed};
		auto registry = RegistryFactory::New();
		auto t0 = timer{};

		for(size_t i = 0; i < count; i++)
		{
			Entity e = registry->Create();
			registry->Emplace<Comp<1>>(e, "Comp1");
			if(proba(0.25))
				registry->Emplace<Comp<2>>(e, "Comp2");
			if(proba(0.25))
				registry->Emplace<Comp<3>>(e, "Comp3");
			if(proba(0.25))
				registry->Emplace<Comp<4>>(e, "Comp4");
			if(proba(0.25))
				registry->Emplace<Comp<5>>(e, "Comp5");
			if(proba(0.25))
				registry->Emplace<Comp<6>>(e, "Comp6");
			if(proba(0.25))
				registry->Emplace<Comp<7>>(e, "Comp7");
		}

		auto query = registry->Select({"Comp1", "Comp3", "Comp5", "Comp7"});
		auto it = query.Having("Comp1 & Comp3 & Comp5 & Comp7");
		it.Each([&](Entity e, Comp<1>* comp1, Comp<3>* comp3, Comp<5>* comp5, Comp<7>* comp7) {
			comp1->a[0] = 1;
			comp3->a[0] = 1;
			comp5->a[0] = 1;
			comp7->a[0] = 1;
		});
		t0.elapsed();
	}
};

class EnTT : public BenchmarkTarget
{
	size_t count;

public:
	EnTT(size_t count)
	{
		this->count = count;
	}

	virtual void Scenario1(unsigned int seed) override
	{
		auto proba = UniformRandom<float>{seed};
		auto registry = entt::registry{};
		auto t0 = timer{};

		for(size_t i = 0; i < count; i++)
		{
			auto e = registry.create();
			registry.emplace<Comp<1>>(e, Comp<1>());
		}
		t0.elapsed();
		timer t1;
		{
			auto view = registry.view<Comp<1>>();
			view.each([&](auto e, auto& comp1) {
				comp1.a[0] = 1;
				if(proba(0.25))
					registry.emplace<Comp<2>>(e);
				if(proba(0.25))
					registry.emplace<Comp<3>>(e);
				if(proba(0.25))
					registry.emplace<Comp<4>>(e);
				if(proba(0.25))
					registry.emplace<Comp<5>>(e);
			});
		}
		t1.elapsed();
		timer t2;
		{
			auto view = registry.view<Comp<2>, Comp<3>>();
			view.each([&](auto e, auto& comp2, auto& comp3) {
				comp2.a[0] = 1;
				comp3.a[0] = 1;
				if(proba(0.25))
					registry.remove<Comp<2>>(e);
				if(proba(0.25))
					registry.remove<Comp<3>>(e);
			});
		}
		t2.elapsed();
		timer t3;
		{
			auto view = registry.view<Comp<4>, Comp<5>>();
			view.each([&](auto e, auto& comp4, auto& comp5) {
				comp4.a[0] = 1;
				comp5.a[0] = 1;
				if(proba(0.25))
					registry.remove_all(e);
			});
		}
		t3.elapsed();
		timer t4;
		{
			auto view = registry.view<Comp<1>>();
			view.each([&](auto e, auto& comp1) {
				comp1.a[0] = 1;
				//if(RandProb(0.25))
				registry.remove_all(e);
			});
		}
		t4.elapsed();
	}

	virtual void Scenario2(unsigned int seed) override
	{
		auto proba = UniformRandom<float>{seed};
		auto registry = entt::registry{};
		auto t0 = timer{};

		for(size_t i = 0; i < count; i++)
		{
			auto e = registry.create();
			registry.emplace<Comp<1>>(e, Comp<1>());
			if(proba(0.25))
				registry.emplace<Comp<2>>(e);
			if(proba(0.25))
				registry.emplace<Comp<3>>(e);
			if(proba(0.25))
				registry.emplace<Comp<4>>(e);
			if(proba(0.25))
				registry.emplace<Comp<5>>(e);
			if(proba(0.25))
				registry.emplace<Comp<6>>(e);
			if(proba(0.25))
				registry.emplace<Comp<7>>(e);
		}

		auto view = registry.view<Comp<1>, Comp<3>, Comp<5>, Comp<7>>();
		view.each([&](auto e, auto& comp1, auto& comp3, auto& comp5, auto& comp7) {
			comp1.a[0] = 1;
			comp3.a[0] = 1;
			comp5.a[0] = 1;
			comp7.a[0] = 1;
		});
		t0.elapsed();
	}
};

} // namespace entidy_benchmark
int main(int argc, char** argv)
{
	using namespace entidy_benchmark;

	size_t count = 10000000;

	cout << "Scenario 1" << endl;
	cout << "OURS" << endl;
	{
		shared_ptr<BenchmarkTarget> ours = make_shared<Entidy>(count);
		ours->Scenario1(1);
	}
	std::this_thread::sleep_for(1s);
	cout << "ENTT" << endl;
	{
		shared_ptr<BenchmarkTarget> entt = make_shared<EnTT>(count);
		entt->Scenario1(1);
	}

	std::this_thread::sleep_for(1s);

	cout << "Scenario 2" << endl;
	cout << "OURS" << endl;
	{
		shared_ptr<BenchmarkTarget> ours = make_shared<Entidy>(count);
		ours->Scenario2(1);
	}
	std::this_thread::sleep_for(1s);
	cout << "ENTT" << endl;
	{
		shared_ptr<BenchmarkTarget> entt = make_shared<EnTT>(count);
		entt->Scenario2(1);
	}

	return 0;
}
