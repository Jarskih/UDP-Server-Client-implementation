#pragma once
#include "Scene.h"

namespace charlie
{
	struct End : Scene
	{
		End();
		bool on_init(SDL_Renderer* renderer) override;
		void on_exit() override;
		bool on_tick(const Time& dt) override;
		void on_draw() override;
		SDLSprite* sprite_;
		SDL_Renderer* renderer_;
	};
}
