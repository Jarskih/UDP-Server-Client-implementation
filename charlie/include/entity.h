#pragma once
#include "charlie_gameplay.hpp"
#include "sdl_sprite.hpp"

namespace charlie
{
	struct Entity {
		Entity();
		~Entity();
		Entity& operator=(const Entity& rhs);
		void init(SDL_Renderer* renderer, Vector2& pos, uint32 id);
		void load_body_sprite(std::string body, int srcX, int srcY, int srcW, int srcH);
		void load_turret_sprite(std::string turret, int srcX, int srcY, int srcW, int srcH);
		void render(SDL_Rect cam);
		// SDL
		SDL_Renderer* renderer_;
		SDLSprite* body_sprite_;
		SDL_Rect body_window_rect_;
		SDLSprite* turret_sprite_;
		SDL_Rect turret_window_rect_;
		SDL_Point point_;

		Transform transform_;
		uint32 id_;
		gameplay::Interpolator interpolator_;
		float turret_rotation_;
	};
}
