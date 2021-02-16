
#include <iostream>

#include <roaring.hh>
#include <roaring64map.hh>

#include <entidy/Entidy.h>
#include <entidy/QueryParser.h>
#include <entidy/MemoryManager.h>
#include <entidy/Query.h>

using namespace std;
using namespace entidy;

struct Vec3
{
	Vec3(int x, int y, int z)
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

class QPAdapter : public QueryParserAdapter<int>
{
public:
	virtual int Evaluate(const string& token)
	{
		return stoi(token);
	}

	virtual int And(const int& lhs, const int& rhs)
	{
		return lhs * rhs;
	}

	virtual int Or(const int& lhs, const int& rhs)
	{
		return lhs + rhs;
	}

	virtual int Not(const int& rhs)
	{
		return -rhs;
	}
};

int main()
{
	Registry registry = RegistryFactory::New();

	Entity e1 = registry->Create();
    cout << e1 << endl;
	registry->Emplace<Vec3>(e1, "position", 1, 2, 3);

	Entity e2 = registry->Create();
    cout << e2 << endl;
	registry->Emplace(e2, "position", Vec3(7, 8, 9));
	registry->Emplace(e2, "velocity", Vec3(4, 5, 6));

    auto p1 = registry->Component<Vec3>(e1, "position");
    auto p2 = registry->Component<Vec3>(e2, "velocity");

    p1->print();
    p2->print();

	registry->Erase(e2, "position");
	registry->Erase(e1);

	Entity e3 = registry->Create();
    cout << e3 << endl;

	// auto query = registry.Select("pos", "vel");
	// auto it1 = query.Filter("pos & !(vel & stationary) | flying");
	// auto it2 = query.Filter();

	// auto it = registry.Select("pos", "vel").Filter();

	// it.Each([](Entity e, Vec3 pos, Vec3 vel)
	// {
	//     cout << pos.x << " " << vel.dx << "\n";
	// });

	// registry->Erase(e, "pos", "vel");
	// registry->Erase(e);

	//Query q ("a", "b", "c");

	// shared_ptr<QueryParserAdapter<int>> adapter = make_shared<QPAdapter>();
	// QueryParser parser(adapter);

	// // 1 * -(2 + 5) * 66 + 32 * -4 = -590
	// int out = parser.Parse("(1 & !(2 | (5&2)) & 66 | 32 & ! 4)");
	// cout << out << endl;

	// return 0;

	// Registry r = RegistryFactory::New();
	// auto qq = Query("test").And("test2").Not("test3");

	//auto a = r->Fetch("test", "test2");

	// a.Each([](Entity e, const Position &pos, Velocity vel)
	// {
	//     cout << pos.x << " " << vel.dx << "\n";
	// });

	// MemoryManager memoryManager;

	// memoryManager.Create<Position>("pos");

	// Position* pos = memoryManager.Pop<Position>("pos");
	// memoryManager.Push("pos", pos);

	// pos->x = 4;
	// pos->y = 6;
	// pos->z = 11;

	// new (component) ComponentImpl (0, pos);

	// cout << component->parent << endl;
	// Position *pp = ((Position*)component->data);
	// cout << pp->x << endl;
	// cout << pp->y << endl;
	// cout << pp->z << endl;
	// cout << component << endl;
	// cout << component->data << endl;

	// Position* pos1 = memoryManager.Pop<Position>("pos");
	// Position* pos2 = memoryManager.Pop<Position>("pos");

	// memoryManager.Push<Position>("pos", pos);
	// memoryManager.Push<Position>("pos", pos1);
	// memoryManager.Push<Position>("pos", pos2);

	// MemoryManager memoryManager(0, 10);
	// memoryManager.CreatePool<Position>("pos");
	// memoryManager.CreatePool<Velocity>("vel");

	// Component c1 = memoryManager.Pop("pos");
	// Position p = c1->Data<Position>();
	// cout << p.x << endl;

	// c1->Data<Position>().x = 5;
	// c1->Data<Position>().y = 6;
	// c1->Data<Position>().z = 7;
	// cout << c1->Data<Position>().x << endl;
	// c1->Clear();
	// cout << c1->Data<Position>().x << endl;

	// cout << p.x << endl;

	// Component c2 = memoryManager.Pop("pos");
	// cout << c2->Data<Position>().x << endl;
	// c2->Data<Position>().y = 1;
	// c2->Data<Position>().z = 2;

	// memoryManager.Push("pos", c1);
	// memoryManager.Push("pos", c2);

	// Component c3 = memoryManager.Pop("vel");
	// c3->Data<Velocity>().dx = 5;
	// c3->Data<Velocity>().dy = 6;
	// c3->Data<Velocity>().dz = 7;
	// c3->Data<Velocity>().speed = 8;

	// memoryManager.Push("vel", c3);

	return 0;
}