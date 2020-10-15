#pragma once
#include <SDL.h>
#include "sdl_keyboard.hpp"
#include "sdl_mouse.hpp"

namespace charlie
{
	struct InputHandler
	{
		SDLKeyboard m_keyboard;
		SDLMouse m_mouse;

		bool IsKeyDown(SDL_Scancode p_key);
		bool IsKeyPressed(SDL_Scancode p_key);
		bool IsKeyReleased(SDL_Scancode p_key);
		bool IsMouseButtonDown(unsigned int p_button);
		bool IsMouseButtonPressed(unsigned int p_button);
		bool IsMouseButtonReleased(unsigned int p_button);
		int  GetMouseMotionX();
		int  GetMouseMotionY();
		int  GetMousePositionX();
		int  GetMousePositionY();
		int  GetMouseWheelMotionX();
		int  GetMouseWheelMotionY();
		int  GetMouseWheelPositionX();
		int  GetMouseWheelPositionY();
		void HandleEvents();
	};
}