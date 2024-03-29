﻿#pragma once

#include "game.h"
#include "Scene.h"

namespace charlie
{
	struct Menu : Scene
	{
		Menu();
		bool on_init(SDL_Renderer* renderer) override;
		void on_exit() override;
		bool on_tick(const Time& dt) override;
		void on_draw() override;

		SDLSprite* sprite_;
		SDL_Renderer* renderer_;
		bool servers_available_;
	};
}