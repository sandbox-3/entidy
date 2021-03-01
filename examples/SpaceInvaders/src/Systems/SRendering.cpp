#include "Systems/SRendering.h"

using namespace entidy;
using namespace entidy::spaceinvaders;

void SRendering::Init(Engine engine) { }

void SRendering::Update(Engine engine)
{
	if(engine->Renderer()->Cols() < VIEWPORT_W || engine->Renderer()->Cols() < VIEWPORT_H)
	{
		RenderInvalidSize(engine);
		return;
	}

	RenderBackground(engine);
	RenderExplosions(engine);

	RenderSprites(engine);
	ClearBorder(engine);
}

double SRendering::Distance(const Vec2f& p1, const Vec2f& p2)
{
	return sqrt((p1.x - p2.x) * (p1.x - p2.x) + 3 * (p1.y - p2.y) * (p1.y - p2.y));
}

void SRendering::ClearBorder(Engine engine)
{
	ushort offset_x = (engine->Renderer()->Cols() - VIEWPORT_W) / 2;
	ushort offset_y = (engine->Renderer()->Rows() - VIEWPORT_H) / 2;
	for(ushort x = 0; x < engine->Renderer()->Cols(); x++)
	{
		for(ushort y = 0; y < engine->Renderer()->Rows(); y++)
		{
			bool inboundsx = (x >= offset_x && x < offset_x + VIEWPORT_W);
			bool inboundsy = (y >= offset_y && y < offset_y + VIEWPORT_H);

			if(inboundsx && inboundsy)
				continue;

			Pixel* p = engine->Renderer()->At(x, y);
			p->Foreground.R = 0;
			p->Foreground.G = 0;
			p->Foreground.B = 0;
			p->Background.R = 0;
			p->Background.G = 0;
			p->Background.B = 0;
			p->Glyph = ' ';
		}
	}
}

void SRendering::RenderSprites(Engine engine)
{
	ushort offset_x = (engine->Renderer()->Cols() - VIEWPORT_W) / 2;
	ushort offset_y = (engine->Renderer()->Rows() - VIEWPORT_H) / 2;

	auto sprite_view = engine->Registry()->Select({"Sprite", "Position"}).Having("Sprite & Position");
	sprite_view.Each([&](Entity e, Sprite* sprite, Vec2f* position) {
		ushort start_x = offset_x + position->x - floor(float(sprite->cols) / 2.0);
		ushort start_y = offset_y + position->y - floor(float(sprite->rows) / 2.0);
		for(ushort x = 0; x < sprite->cols; x++)
		{
			for(ushort y = 0; y < sprite->rows; y++)
			{
                // if(start_x + x >= VIEWPORT_W || start_x + x < 0)
                //     continue;
                
                // if(start_y + y >= VIEWPORT_H || start_y + y < 0)
                //     continue;
                
				Pixel* p = engine->Renderer()->At(start_x + x, start_y + y);
				size_t frame_id = size_t(round(sprite->frame)) % sprite->frames.size();
				size_t glyph_id = y * sprite->cols + x;
				if(glyph_id > sprite->frames[frame_id].glyphs.size())
					continue;

				if(sprite->frames[frame_id].bgcolor.R >= 0)
					p->Background.R = sprite->frames[frame_id].bgcolor.R;
				if(sprite->frames[frame_id].bgcolor.G >= 0)
					p->Background.G = sprite->frames[frame_id].bgcolor.G;
				if(sprite->frames[frame_id].bgcolor.B >= 0)
					p->Background.B = sprite->frames[frame_id].bgcolor.B;

				if(sprite->frames[frame_id].fgcolor.R >= 0)
					p->Foreground.R = sprite->frames[frame_id].fgcolor.R;
				if(sprite->frames[frame_id].fgcolor.G >= 0)
					p->Foreground.G = sprite->frames[frame_id].fgcolor.G;
				if(sprite->frames[frame_id].fgcolor.B >= 0)
					p->Foreground.B = sprite->frames[frame_id].fgcolor.B;

				p->Glyph = sprite->frames[frame_id].glyphs[glyph_id];
			}
		}
		sprite->frame += sprite->speed;
	});
}

void SRendering::RenderInvalidSize(Engine engine)
{
	string error_msg = "Terminal window size should be at least " + to_string(VIEWPORT_W) + "x" + to_string(VIEWPORT_H);

	ushort offset_x = (engine->Renderer()->Cols() - error_msg.length()) / 2;
	ushort offset_y = engine->Renderer()->Rows() / 2;

	for(ushort x = 0; x < error_msg.length(); x++)
	{
		Pixel* p = engine->Renderer()->At(offset_x + x, offset_y);
		p->Foreground.R = 255;
		p->Foreground.G = 255;
		p->Foreground.B = 255;
		p->Glyph = error_msg[x];
	}
}

void SRendering::RenderBackground(Engine engine)
{
	vector<Vec2f*> fx_fog_positions;
	View view_fog = engine->Registry()->Select({"Position"}).Having("Position & BGFXFog");
	view_fog.Each([&](Entity e, Vec2f* position) { fx_fog_positions.push_back(position); });

	vector<BGFXRipple*> fx_ripples;
	View view_ripple = engine->Registry()->Select({"BGFXRipple"}).Having("BGFXRipple");
	view_ripple.Each([&](Entity e, BGFXRipple* ripple) { fx_ripples.push_back(ripple); });

	for(ushort x = 0; x < engine->Renderer()->Cols(); x++)
	{
		for(ushort y = 0; y < engine->Renderer()->Rows(); y++)
		{
			double dist_fog = 0;
			for(Vec2f* p : fx_fog_positions)
				dist_fog += Distance(*p, Vec2f((double)x, (double)y));
			dist_fog = dist_fog / (double)fx_fog_positions.size();

			double diag = sqrt((VIEWPORT_W * VIEWPORT_W) + (VIEWPORT_H * VIEWPORT_H));
			double sat_fog = (dist_fog / diag);

			Pixel* p = engine->Renderer()->At(x, y);
			p->Background.R = 0;
			p->Background.G = 0;
			p->Background.B = (uint8_t)(sat_fog * 100.0);
		}
	}

	// View view2 = engine->Entidy()->Select({"Position"}).Filter("BGFXFog");
	// view2.Each([&](Entity e, Vec2f* position) {

	// 		Pixel* p = engine->Renderer()->At((uint8_t)position->x, (uint8_t)position->y);
	// 		p->Background.R = 255;
	// 		p->Background.G = 0;
	// 		p->Background.B = 0;
	// 		p->Foreground.R = 0;
	// 		p->Foreground.G = 0;
	// 		p->Foreground.B = 0;
	// 		p->Glyph = 'X';
	//          });
}

void SRendering::RenderExplosions(Engine engine)
{
	ushort offset_x = (engine->Renderer()->Cols() - VIEWPORT_W) / 2;
	ushort offset_y = (engine->Renderer()->Rows() - VIEWPORT_H) / 2;

	vector<BGFXRipple*> fx_ripples;
	View view_ripple = engine->Registry()->Select({"BGFXRipple"}).Having("BGFXRipple");
	view_ripple.Each([&](Entity e, BGFXRipple* ripple) { fx_ripples.push_back(ripple); });

	for(ushort x = 0; x < engine->Renderer()->Cols(); x++)
	{
		for(ushort y = 0; y < engine->Renderer()->Rows(); y++)
		{
			double sat_ripple = 0;
			for(BGFXRipple* p : fx_ripples)
			{
				double dist_ripple = Distance(p->center, Vec2f((double)x, (double)y));
				bool ripple_selected = dist_ripple < (p->radius);
				sat_ripple += ripple_selected ? p->intensity : 0;
			}

			Pixel* p = engine->Renderer()->At(offset_x + x, offset_y + y);
			p->Foreground.R = (uint8_t)(sat_ripple * 75.0);
			p->Foreground.G = (uint8_t)(sat_ripple * 25.0);
			p->Foreground.B = (uint8_t)(max(p->Background.B, (uint8_t)(sat_ripple * 25.0)));
			p->Glyph = sat_ripple > 0 ? (Helper::RandBool(0.5) ? '{' : '}') : ' ';
		}
	}
}