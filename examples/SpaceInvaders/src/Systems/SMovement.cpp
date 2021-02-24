#include "Systems/SMovement.h"

using namespace entidy;
using namespace entidy::spaceinvaders;

void SMovement::Init(Engine engine) { }

void SMovement::Update(Engine engine)
{
	View view_bounce = engine->Entidy()->Select({"Position", "Velocity", "BoundaryAction"}).Having("Position & Velocity & BoundaryAction");
	view_bounce.Each([&](Entity e, Vec2f* position, Vec2f* velocity, BoundaryAction* boundary_action) {
		position->x += velocity->x;
		position->y += velocity->y;

		if(*boundary_action == BoundaryAction::BOUNCE)
		{
			if(position->x > VIEWPORT_W)
				velocity->x = -velocity->x;
			if(position->x < 0)
				velocity->x = -velocity->x;

			if(position->y > VIEWPORT_H)
				velocity->y = -velocity->y;
			if(position->y < 0)
				velocity->y = -velocity->y;
		}
		else if(*boundary_action == BoundaryAction::WARP)
		{
			if(position->x > VIEWPORT_W)
				position->x = 0;
			if(position->x < 0)
				position->x = VIEWPORT_W;

			if(position->y > VIEWPORT_H)
				position->y = 0;
			if(position->y < 0)
				position->y = VIEWPORT_H;
		}
        else
        {
			bool inbounds = (position->x < VIEWPORT_W && position->x > 0);
            inbounds &= (position->y < VIEWPORT_H && position->y > 0);

            if(!inbounds)
                engine->Entidy()->Erase(e);
        }
	});
}