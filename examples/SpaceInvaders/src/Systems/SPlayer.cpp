#include "Systems/SPlayer.h"

using namespace entidy;
using namespace entidy::spaceinvaders;

void SPlayer::Init(Engine engine)
{
	Entity e = engine->Entidy()->Create();
	engine->Entidy()->Emplace<Vec2f>(e, "Position", VIEWPORT_W / 2, VIEWPORT_H - 1);
	engine->Entidy()->Emplace<Sprite>(e, "Sprite", SpriteFactory::Player());
	engine->Entidy()->Emplace<u_int8_t>(e, "Player");
}

void SPlayer::Update(Engine engine)
{

	auto input_cmd_view = engine->Entidy()->Select({"InputCommand"}).Having("InputCommand");
	input_cmd_view.Each([&](Entity cmd_e, InputCommand* cmd) {

		auto player_view = engine->Entidy()->Select({"Position", "Sprite"}).Having("Player & Position & Sprite");
		player_view.Each([&](Entity player_e, Vec2f* position, Sprite * sprite) {
            
			if(*cmd == InputCommand::A)
			{
				if(position->x > floor(float(sprite->cols) / 2.0))
					position->x--;
			}
			else if(*cmd == InputCommand::D)
			{
				if(position->x < VIEWPORT_W - ceil(float(sprite->cols) / 2.0))
					position->x++;
			}
			else if(*cmd == InputCommand::SPACE)
			{
	            Entity bullet = engine->Entidy()->Create();
	            engine->Entidy()->Emplace<Vec2f>(bullet, "Position", position->x, position->y);
	            engine->Entidy()->Emplace<Vec2f>(bullet, "Velocity", 0, -1);
	            engine->Entidy()->Emplace<BoundaryAction>(bullet, "BoundaryAction", BoundaryAction::DISAPPEAR);
	            engine->Entidy()->Emplace<Sprite>(bullet, "Sprite", SpriteFactory::Bullet());
	            engine->Entidy()->Emplace<int8_t>(bullet, "Bullet");
			}
			else
			{
				return;
			}

			engine->Entidy()->Erase(cmd_e);
		});
	});
}