#include <iostream>
#include <memory>

#include "entidy/Entidy.h"
#include "entt.hpp"

template <size_t T>
struct Comp
{
	char a[T * 64];
};

class BenchmarkTarget
{
public:
	virtual void Scenario1(unsigned int seed) = 0;
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

class Entidy : public BenchmarkTarget
{

public:
	Entidy() { }

	virtual void Scenario1(unsigned int seed) override
	{
        cout << "0" << endl;
		rand_r(&seed);
		Registry registry = RegistryFactory::New();
		size_t count = 10000;
		for(size_t i = 0; i < count; i++)
		{
			Entity e = registry->Create();
			registry->Emplace(e, "Comp1", Comp<1>());
		}
        cout << "1" << endl;
		{
			auto query = registry->Select("Comp1");
			auto it = query.Filter();
			it.Each([&](Entity e, Comp<1> *comp1) {
                comp1->a[0] = 1;
				if(RandProb(0.25))
					registry->Emplace(e, "Comp2", Comp<2>());
				if(RandProb(0.25))
					registry->Emplace(e, "Comp3", Comp<3>());
				if(RandProb(0.25))
					registry->Emplace(e, "Comp4", Comp<4>());
				if(RandProb(0.25))
					registry->Emplace(e, "Comp5", Comp<5>());
			});
		}
        cout << "2" << endl;
		{
			auto query = registry->Select({"Comp2", "Comp3"});
			auto it = query.Filter();
			it.Each([&](Entity e, Comp<2> *comp2, Comp<3> *comp3){
                comp2->a[0] = comp3->a[0];
				if(RandProb(0.25))
					registry->Erase(e, "Comp2");
				if(RandProb(0.25))
					registry->Erase(e, "Comp3");
			});
		}
        cout << "3" << endl;
		{
			auto query = registry->Select({"Comp4", "Comp5"});
			auto it = query.Filter();
			it.Each([&](Entity e, Comp<4> *comp4, Comp<5> *comp5) {
                comp4->a[0] = comp5->a[0];
				if(RandProb(0.25))
					registry->Erase(e);
			});
		}
        cout << "4" << endl;
		{
			auto query = registry->Select("Comp1");
			auto it = query.Filter();
			it.Each([&](Entity e, Comp<1> *comp1) {
                comp1->a[0] = 1;
				if(RandProb(0.25))
					registry->Erase(e);
			});
		}
	}
};

} // namespace entidy_benchmark
int main(int argc, char** argv)
{
    using namespace entidy_benchmark;

    shared_ptr<BenchmarkTarget> ours = make_shared<Entidy>();

    ours->Scenario1(1);
	return 0;
}