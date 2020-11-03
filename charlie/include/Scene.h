#pragma once
#include <sdl_application.hpp>

namespace charlie {
	namespace network {
		struct NetworkStreamWriter;
		struct NetworkStreamReader;
		struct Connection;
	}

	struct Time;

	struct Scene
	{
		virtual bool on_init(SDL_Renderer* renderer) = 0;
		virtual void on_exit() = 0;
		virtual bool on_tick(const Time& dt) = 0;
		virtual void on_draw() = 0;

		virtual ~Scene() = default;
	};
}
