#pragma once
#include <SDL2/SDL.h>

struct SDLKeyboard
{
	bool m_currentFrame[SDL_NUM_SCANCODES];
	bool m_lastFrame[SDL_NUM_SCANCODES];
	SDLKeyboard();
	~SDLKeyboard();
	void Update();
	bool IsKeyDown(SDL_Scancode p_key);
	bool IsKeyPressed(SDL_Scancode p_key);
	bool IsKeyReleased(SDL_Scancode p_key);
	void UpdateKey(SDL_Scancode p_key, bool p_value);
};
