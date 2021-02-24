#pragma once
#include "Engine.h"
#include "Components.h"
#include "Helper.h"

namespace entidy::spaceinvaders
{
using namespace std;
using namespace entidy;

class SBackground : public System
{
public:
	virtual void Init(Engine engine) override;
	virtual void Update(Engine engine) override;
};

}