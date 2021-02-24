#pragma once
#include "Components.h"
#include "Engine.h"
#include "Helper.h"

namespace entidy::spaceinvaders
{
using namespace std;
using namespace entidy;

class SMovement : public System
{
protected:

public:
	virtual void Init(Engine engine) override;
	virtual void Update(Engine engine) override;
};

} // namespace entidy::spaceinvaders