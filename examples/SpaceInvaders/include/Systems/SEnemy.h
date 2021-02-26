#pragma once
#include "Components.h"
#include "Engine.h"
#include "Helper.h"
#include "SpriteFactory.h"

namespace entidy::spaceinvaders
{
using namespace std;
using namespace entidy;

class SEnemy : public System
{
protected:
    bool Collision(int x, int y, int target_x, int target_y, int target_w, int target_h);

public:
	virtual void Init(Engine engine) override;
	virtual void Update(Engine engine) override;
};

} // namespace entidy::spaceinvaders