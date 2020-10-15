﻿#pragma once
#include "charlie_gameplay.hpp"
#include "sdl_sprite.hpp"

namespace charlie
{
	struct Entity {
		Entity();
		void init(SDL_Renderer* renderer, Vector2& pos, uint32 id);
		void load_body_sprite(const char* body, int srcX, int srcY, int srcW, int srcH);
		void load_turret_sprite(const char* turret, int srcX, int srcY, int srcW, int srcH);
		void render();
		// SDL
		SDL_Renderer* renderer_;
		SDLSprite* body_sprite_;
		SDL_Rect body_window_rect_;
		SDLSprite* turret_sprite_;
		SDL_Rect turret_window_rect_;

		Transform transform_;
		uint32 id_;
		gameplay::Interpolator interpolator_;

	};
}
