#pragma once
#include "sdl_sprite.hpp"
#include "sdl_window.hpp"

namespace charlie
{
	struct Vector2;

	struct Player {
		Player();
		void init(SDL_Renderer* renderer, Vector2& pos, uint32 id);

		void update(Time tickrate);
		void render();
		void load_sprite(const char* path, int p_srcX, int p_srcY, int p_srcW, int p_srcH);
		uint8 get_input_bits() const;

		// SDL
		SDL_Rect window_rect_;
		SDL_Renderer* renderer_;
		SDLSprite* sprite_;

		Vector2 position_;
		uint8 input_bits_;
		uint32 id_;
		float speed_;

	};
}
