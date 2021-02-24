
#include <iostream>
#include <entidy/Entidy.h>

#include "Engine.h"

#include "Systems/SInput.h"
#include "Systems/SMovement.h"
#include "Systems/SBackground.h"
#include "Systems/SRendering.h"
#include "Systems/SPlayer.h"


using namespace std;
using namespace entidy;
using namespace entidy::spaceinvaders;

int main()
{
    Engine engine = make_shared<EngineImpl>(30);
    engine->AddSystem(make_shared<SInput>());
    engine->AddSystem(make_shared<SMovement>());
    engine->AddSystem(make_shared<SBackground>());
    engine->AddSystem(make_shared<SRendering>());
    engine->AddSystem(make_shared<SPlayer>());
    engine->Run();
	return 0;
}