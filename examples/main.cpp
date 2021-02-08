
#include <iostream>

#include <entidy/Entidy.h>

#include <entidy/MemoryManager/MemoryManager.h>

using namespace std;
using namespace entidy;

struct Position
{
    Position()
    {

    }
    
    Position(int x, int y, int z)
    {

    }

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

int main()
{
    //Registry r = RegistryFactory::NewRegistry();
    //r->NewIndex("key1")->And("key2")->Not("Key3")->Register();
    //Entity entity = r->NewEntity();

    MemoryManager memoryManager;
    
    memoryManager.Create<ComponentImpl>("comp");
    memoryManager.Create<Position>("pos");

    Position* pos = memoryManager.Pop<Position>("pos");
    Component component = memoryManager.Pop<ComponentImpl>("comp");

    pos->x = 4;
    pos->y = 6;
    pos->z = 11;
    
    new (component) ComponentImpl (0, pos);
    
    cout << component->parent << endl;
    Position *pp = ((Position*)component->data);
    cout << pp->x << endl;
    cout << pp->y << endl;
    cout << pp->z << endl;
    cout << component << endl;
    cout << component->data << endl;

    Position* pos1 = memoryManager.Pop<Position>("pos");
    Position* pos2 = memoryManager.Pop<Position>("pos");

    memoryManager.Push<Position>("pos", pos);
    memoryManager.Push<Position>("pos", pos1);
    memoryManager.Push<Position>("pos", pos2);


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