
#include <chrono>
#include <iostream>
#include <memory>

#include <entidy/CRoaring/roaring.hh>
#include <entidy/Entidy.h>
#include <entidy/MemoryManager.h>
#include <entidy/Query.h>
#include <entidy/QueryParser.h>

#include <entidy/SparseVector.h>

using namespace std;
using namespace entidy;

struct Vec3
{
	Vec3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	float x;
	float y;
	float z;

	void print()
	{
		cout << "x=" << x << endl;
		cout << "y=" << y << endl;
		cout << "z=" << z << endl;
	}
};

struct Position
{
	int x;
	int y;
	int z;
};

struct Velocity
{
	int dx;
	int dy;
	int dz;

	float speed;
};

struct A
{
	size_t a[256];
};

int main()
{
	Entidy entidy;
	Entity e = entidy.Create();
	entidy.Emplace<Position>(e, "test1");
	entidy.Emplace<Velocity>(e, "test2");
	entidy.Emplace<Vec3>(e, "test3", 1,2,3);
	entidy.Emplace(e, "flag");
    entidy.Commit();

	auto view = entidy.Select({"test1", "test3"}).Having("test1 & test3 & flag");
	view.Each([&](Entity n, Position* pos, Vec3* vel) { vel->print(); });

	return 0;
}
