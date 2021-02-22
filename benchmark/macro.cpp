#include <chrono>
#include <iostream>
#include <memory>

#include "entidy/Entidy.h"
#include "entt.hpp"

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

size_t RandInt(size_t min, size_t max)
{
	return (rand() % (max - min)) + min;
}

float RandFloat()
{
	return float(rand() % 10000) / 10000.0f;
}

bool RandProb(float p)
{
	return (float(rand() % 10000) / 10000.0f) <= p;
}

namespace entidy_benchmark
{
using namespace entidy;
using namespace entt;

class Entidy : public BenchmarkTarget
{
	size_t count;

public:
	Entidy(size_t count) { this->count = count;}

	virtual void Scenario1(unsigned int seed) override
	{
		rand_r(&seed);
		Registry registry = RegistryFactory::New();
		timer t0;
		for(size_t i = 0; i < count; i++)
		{
			Entity e = registry->Create();
			registry->Emplace<Comp<1>>(e, "Comp1");
		}
		t0.elapsed();
		timer t1;
		{
			auto query = registry->Select({"Comp1"});
			auto it = query.Filter();
			it.Each([&](Entity e, Comp<1>* comp1) {
				comp1->a[0] = 1;
				if(RandProb(0.25))
					registry->Emplace<Comp<2>>(e, "Comp2");
				if(RandProb(0.25))
					registry->Emplace<Comp<3>>(e, "Comp3");
				if(RandProb(0.25))
					registry->Emplace<Comp<4>>(e, "Comp4");
				if(RandProb(0.25))
					registry->Emplace<Comp<5>>(e, "Comp5");
			});
		}
		t1.elapsed();
		timer t2;
		{
			auto query = registry->Select({"Comp2", "Comp3"});
			auto it = query.Filter();
			it.Each([&](Entity e, Comp<2>* comp2, Comp<3>* comp3) {
				comp2->a[0] = comp3->a[0];
				if(RandProb(0.25))
					registry->Erase(e, "Comp2");
				if(RandProb(0.25))
					registry->Erase(e, "Comp3");
			});
		}
		t2.elapsed();
		timer t3;
		{
			auto query = registry->Select({"Comp4", "Comp5"});
			auto it = query.Filter();
			it.Each([&](Entity e, Comp<4>* comp4, Comp<5>* comp5) {
				comp4->a[0] = comp5->a[0];
				if(RandProb(0.25))
					registry->Erase(e);
			});
		}
		t3.elapsed();
		timer t4;
		{
			auto query = registry->Select({"Comp1"});
			auto it = query.Filter();
			it.Each([&](Entity e, Comp<1>* comp1) {
				comp1->a[0] = 1;
				//if(RandProb(0.25))
					registry->Erase(e);
			});
		}
		t4.elapsed();

    registry->CleanUp();
    int a;
    cin >> a;
	}

	virtual void Scenario2(unsigned int seed) override
	{
		rand_r(&seed);
		Registry registry = RegistryFactory::New();
		for(size_t i = 0; i < count; i++)
		{
			Entity e = registry->Create();
			registry->Emplace<Comp<1>>(e, "Comp1");
			if(RandProb(0.25))
				registry->Emplace<Comp<2>>(e, "Comp2");
			if(RandProb(0.25))
				registry->Emplace<Comp<3>>(e, "Comp3");
			if(RandProb(0.25))
				registry->Emplace<Comp<4>>(e, "Comp4");
			if(RandProb(0.25))
				registry->Emplace<Comp<5>>(e, "Comp5");
			if(RandProb(0.25))
				registry->Emplace<Comp<6>>(e, "Comp6");
			if(RandProb(0.25))
				registry->Emplace<Comp<7>>(e, "Comp7");
		}

		timer t0;
			auto query = registry->Select({"Comp1", "Comp3", "Comp5", "Comp7"});
			auto it = query.Filter();
			it.Each([&](Entity e, Comp<1>* comp1, Comp<3>* comp3, Comp<5>* comp5, Comp<7>* comp7) {
				comp1->a[0] = 1;
			});
		t0.elapsed();
	}
};

class EnTT : public BenchmarkTarget
{
	size_t count;

public:
	EnTT(size_t count) { this->count = count;}

	virtual void Scenario1(unsigned int seed) override
	{
		rand_r(&seed);
		entt::registry registry;

		timer t0;
		for(size_t i = 0; i < count; i++)
		{
			auto e = registry.create();
			registry.emplace<Comp<1>>(e, Comp<1>());
		}
		t0.elapsed();
		timer t1;
		{
			auto view = registry.view<Comp<1>>();
			view.each([&](auto e, auto comp1) {
				comp1.a[0] = 1;
				if(RandProb(0.25))
					registry.emplace<Comp<2>>(e);
				if(RandProb(0.25))
					registry.emplace<Comp<3>>(e);
				if(RandProb(0.25))
					registry.emplace<Comp<4>>(e);
				if(RandProb(0.25))
					registry.emplace<Comp<5>>(e);
			});
		}
		t1.elapsed();
		timer t2;
		{
			auto view = registry.view<Comp<2>, Comp<3>>();
			view.each([&](auto e, auto &comp2, auto &comp3) {
				comp2.a[0] = comp3.a[0];
				if(RandProb(0.25))
					registry.remove<Comp<2>>(e);
				if(RandProb(0.25))
					registry.remove<Comp<3>>(e);
			});
		}
		t2.elapsed();
		timer t3;
		{
			auto view = registry.view<Comp<4>, Comp<5>>();
			view.each([&](auto e, auto &comp4, auto &comp5) {
				comp4.a[0] = comp5.a[0];
				if(RandProb(0.25))
					registry.remove_all(e);
			});
		}
		t3.elapsed();
		timer t4;
		{
			auto view = registry.view<Comp<1>>();
			view.each([&](auto e, auto &comp1) {
				comp1.a[0] = 1;
				//if(RandProb(0.25))
					registry.remove_all(e);
			});
		}
		t4.elapsed();
        
	}

	virtual void Scenario2(unsigned int seed) override
	{
		rand_r(&seed);
		entt::registry registry;

		for(size_t i = 0; i < count; i++)
		{
			auto e = registry.create();
			registry.emplace<Comp<1>>(e, Comp<1>());
				if(RandProb(0.25))
					registry.emplace<Comp<2>>(e);
				if(RandProb(0.25))
					registry.emplace<Comp<3>>(e);
				if(RandProb(0.25))
					registry.emplace<Comp<4>>(e);
				if(RandProb(0.25))
					registry.emplace<Comp<5>>(e);
				if(RandProb(0.25))
					registry.emplace<Comp<6>>(e);
				if(RandProb(0.25))
					registry.emplace<Comp<7>>(e);
		}

		timer t0;
			auto view = registry.view<Comp<1>, Comp<3>, Comp<5>, Comp<7>>();
			view.each([&](auto e, auto &comp1, auto &comp3, auto &comp5, auto &comp7) {
				comp1.a[0] = 1;
			});
		t0.elapsed();
	}
};

} // namespace entidy_benchmark
int main(int argc, char** argv)
{
	using namespace entidy_benchmark;

    size_t count = 10000000;

	cout << "OURS" << endl;
	{
		shared_ptr<BenchmarkTarget> ours = make_shared<Entidy>(count);
		ours->Scenario1(1);
	}
	cout << "ENTT" << endl;
	{
	    shared_ptr<BenchmarkTarget> entt = make_shared<EnTT>(count);
	 	entt->Scenario1(1);
	}
	return 0;
}