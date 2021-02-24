#pragma once
#include "Components.h"
#include "Engine.h"
#include "Helper.h"

namespace entidy::spaceinvaders
{
using namespace std;
using namespace entidy;

class SRendering : public System
{
protected:
    void RenderPlayer(Engine engine);
	void RenderInvalidSize(Engine engine);
	void RenderBackground(Engine engine);
    void RenderProjectiles(Engine engine);
    
	double Distance(const Vec2f& p1, const Vec2f& p2);

public:
	virtual void Init(Engine engine) override;
	virtual void Update(Engine engine) override;
};

} // namespace entidy::spaceinvaders