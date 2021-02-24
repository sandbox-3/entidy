#include "Systems/SInput.h"

using namespace entidy;
using namespace entidy::spaceinvaders;

void SInput::Init(Engine engine) { }

void SInput::Update(Engine engine)
{
	auto input_buffer = engine->InputBuffer();

	while(input_buffer->size() > 0)
	{
		char input_char = input_buffer->front();
		input_buffer->pop_front();

		InputCommand cmd;
		switch(input_char)
		{
		case 'A':
		case 'a':
			cmd = InputCommand::A;
			break;
		case 'D':
		case 'd':
			cmd = InputCommand::D;
			break;
		case ' ':
			cmd = InputCommand::SPACE;
			break;
		case 'P':
		case 'p':
			cmd = InputCommand::P;
			break;
		case 'Q':
		case 'q':
			cmd = InputCommand::Q;
			break;
		default:
			continue;
		};
		Entity e = engine->Entidy()->Create();
        engine->Entidy()->Emplace(e, "InputCommand", cmd);
	}
	// View view_bounce = engine->Entidy()->Select({"Position", "Velocity"}).Filter("Bounce");
	// view_bounce.Each([&](Entity e, Vec2f* position, Vec2f* velocity) {
	// 	position->x += velocity->x;
	// 	position->y += velocity->y;

	// 	if(position->x > VIEWPORT_W)
	// 		velocity->x = -velocity->x;
	// 	if(position->x < 0)
	// 		velocity->x = -velocity->x;

	// 	if(position->y > VIEWPORT_H)
	// 		velocity->y = -velocity->y;
	// 	if(position->y < 0)
	// 		velocity->y = -velocity->y;
	// });
}