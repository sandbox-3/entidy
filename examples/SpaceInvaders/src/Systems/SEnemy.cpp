#include "Systems/SEnemy.h"

using namespace entidy;
using namespace entidy::spaceinvaders;

void SEnemy::Init(Engine engine)
{
	Entity e = engine->Registry()->Create();
	engine->Registry()->Emplace<int>(e, "EnemySpawn", 1);
}

void SEnemy::Update(Engine engine)
{
	auto spawn_view = engine->Registry()->Select({"EnemySpawn"}).Having("EnemySpawn");
    int *spawn = spawn_view.At<int>(0, 0);
    if((*spawn)-- == 0)
    {
        *spawn = 512 + Helper::RandInt(0, 256);

        int count = Helper::RandInt(3, 6);
        float sep_dist = VIEWPORT_W / 5;
        for(int i = 0; i < count; i++)
        {
            float dir = 0.1 * (Helper::RandBool(0.5) ? -1 : 1);
            int enemy_type = Helper::RandInt(0, 3);
            Entity e = engine->Registry()->Create();
            engine->Registry()->Emplace<Vec2f>(e, "Position", sep_dist * i, 3);
            engine->Registry()->Emplace<Sprite>(e, "Sprite", SpriteFactory::Enemy(enemy_type));
            engine->Registry()->Emplace<Vec2f>(e, "Velocity", dir, 0.01);
            engine->Registry()->Emplace<BoundaryAction>(e, "BoundaryAction", BoundaryAction::WARP);
            engine->Registry()->Emplace<u_int8_t>(e, "Enemy");
            engine->Registry()->Emplace<int>(e, "Health", (enemy_type + 1) * 20);
        }
    }

	auto sprite_view = engine->Registry()->Select({"Sprite", "Position", "Health"}).Having("Sprite & Position & Health & Enemy");
	sprite_view.Each([&](Entity enemy, Sprite* sprite, Vec2f* enemy_pos, uint8_t * health) {
		auto bullet_view = engine->Registry()->Select({"Position"}).Having("Bullet & Position");
		bullet_view.Each([&](Entity bullet, Vec2f* bullet_pos) {
            
            if(Collision(bullet_pos->x, bullet_pos->y, enemy_pos->x, enemy_pos->y, sprite->cols, sprite->rows))
            {
                Entity ripple_entity = engine->Registry()->Create();
                BGFXRipple ripple;
                ripple.center = Vec2f(bullet_pos->x, bullet_pos->y);
                ripple.radius = 0;

                engine->Registry()->Erase(bullet);
                (*health)--;
                if(*health == 0)
                {
                    engine->Registry()->Erase(enemy);
                    ripple.intensity = 1;
                }
                else
                {
                    ripple.intensity = Helper::RandDouble(0.1, 0.5);
                }

                engine->Registry()->Emplace(ripple_entity, "BGFXRipple", ripple);
            }
        });
    });
}


    bool SEnemy::Collision(int x, int y, int target_x, int target_y, int target_w, int target_h)
    {
        int min_x = target_x - float(target_w) / 2.0;
        int max_x = target_x + float(target_w) / 2.0;
        int min_y = target_y - float(target_h) / 2.0;
        int max_y = target_y + float(target_h) / 2.0;
        return x >= min_x && x <= max_x && y >= min_y && y <= max_y;
    }