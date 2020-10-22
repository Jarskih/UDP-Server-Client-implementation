#pragma once
#include "charlie.hpp"
#include "sdl_sprite.hpp"

namespace charlie
{
	struct Time;
	struct Camera;
	struct Vector2;

	struct Projectile
	{
		Projectile();
		explicit Projectile(Vector2 pos, Vector2 dir, uint32 owner);
		explicit Projectile(Vector2 pos, float rot, uint32 owner);
		
		void update(Time deltaTime);
		void render(SDL_Rect cameraPos);
		void load_sprite(const char* body, int srcX, int srcY, int srcW, int srcH);
		void destroy();
		
		// SDL
		SDL_Renderer* renderer_;
		SDL_Rect window_rect_;
		SDLSprite* sprite_;
		SDL_Point point;

		Transform transform_;
		uint32 owner_;
		float speed_;
		Vector2 direction_;
	};
}
