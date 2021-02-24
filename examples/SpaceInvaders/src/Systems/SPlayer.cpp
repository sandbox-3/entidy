#include "Systems/SPlayer.h"

using namespace entidy;
using namespace entidy::spaceinvaders;

void SPlayer::Init(Engine engine)
{
	Entity e = engine->Entidy()->Create();
	engine->Entidy()->Emplace<Vec2s>(e, "Position", VIEWPORT_W / 2, VIEWPORT_H - 1);
	engine->Entidy()->Emplace<u_int8_t>(e, "Player");
}

void SPlayer::Update(Engine engine)
{

	auto input_cmd_view = engine->Entidy()->Select({"InputCommand"}).Having("InputCommand");
	input_cmd_view.Each([&](Entity cmd_e, InputCommand* cmd) {

		auto player_view = engine->Entidy()->Select({"Position"}).Having("Player");
		player_view.Each([&](Entity player_e, Vec2s* position) {
            
			if(*cmd == InputCommand::A)
			{
				if(position->x > 0)
					position->x--;
			}
			else if(*cmd == InputCommand::D)
			{
				if(position->x < VIEWPORT_W - 5)
					position->x++;
			}
			else if(*cmd == InputCommand::SPACE)
			{
	            Entity bullet = engine->Entidy()->Create();
	            engine->Entidy()->Emplace<Vec2f>(bullet, "Position", position->x + 2, position->y);
	            engine->Entidy()->Emplace<Vec2f>(bullet, "Velocity", 0, -1);
	            engine->Entidy()->Emplace<BoundaryAction>(bullet, "BoundaryAction", BoundaryAction::DISAPPEAR);
	            engine->Entidy()->Emplace<int8_t>(bullet, "Projectile");
			}
			else
			{
				return;
			}

			engine->Entidy()->Erase(cmd_e);
		});
	});
}