#include "Systems/SBackground.h"

using namespace entidy;
using namespace entidy::spaceinvaders;

void SBackground::Init(Engine engine)
{
	for(int i = 0; i < 3; i++)
	{
		Entity e = engine->Registry()->Create();
		engine->Registry()->Emplace<Vec2f>(e, "Position", VIEWPORT_W/2, VIEWPORT_H/2);
		engine->Registry()->Emplace<Vec2f>(e, "Velocity", Helper::RandDouble(-0.25, 0.25), Helper::RandDouble(-0.25, 0.25));
		engine->Registry()->Emplace<u_int8_t>(e, "BGFXFog");
		engine->Registry()->Emplace<BoundaryAction>(e, "BoundaryAction", BoundaryAction::BOUNCE);
	}
}

void SBackground::Update(Engine engine)
{
	View view_ripple = engine->Registry()->Select({"BGFXRipple"}).Having("BGFXRipple");
	view_ripple.Each([&](Entity e, BGFXRipple* bgfxripple) {
        bgfxripple->intensity -= 0.03;
        bgfxripple->radius += 0.95;
        if(bgfxripple->intensity <= 0)
            engine->Registry()->Erase(e);
	});

}