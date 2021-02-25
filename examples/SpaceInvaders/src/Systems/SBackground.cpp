#include "Systems/SBackground.h"

using namespace entidy;
using namespace entidy::spaceinvaders;

void SBackground::Init(Engine engine)
{
	for(int i = 0; i < 3; i++)
	{
		Entity e = engine->Entidy()->Create();
		engine->Entidy()->Emplace<Vec2f>(e, "Position", VIEWPORT_W/2, VIEWPORT_H/2);
		engine->Entidy()->Emplace<Vec2f>(e, "Velocity", Helper::RandDouble(-0.25, 0.25), Helper::RandDouble(-0.25, 0.25));
		engine->Entidy()->Emplace<u_int8_t>(e, "BGFXFog");
		engine->Entidy()->Emplace<BoundaryAction>(e, "BoundaryAction", BoundaryAction::BOUNCE);
	}
}

void SBackground::Update(Engine engine)
{
	View view_ripple = engine->Entidy()->Select({"BGFXRipple"}).Having("BGFXRipple");
	view_ripple.Each([&](Entity e, BGFXRipple* bgfxripple) {
        bgfxripple->intensity -= 0.03;
        bgfxripple->radius += 0.95;
        if(bgfxripple->intensity <= 0)
            engine->Entidy()->Erase(e);
	});

	if(Helper::RandBool(0.99))
        return;
    
	Entity e = engine->Entidy()->Create();

    BGFXRipple ripple;
    ripple.center = Vec2f ( Helper::RandInt(0, VIEWPORT_W), Helper::RandInt(0, VIEWPORT_H));
    ripple.radius = 0;
    ripple.intensity = Helper::RandDouble(0.5, 1);
    engine->Entidy()->Emplace(e, "BGFXRipple", ripple);

}