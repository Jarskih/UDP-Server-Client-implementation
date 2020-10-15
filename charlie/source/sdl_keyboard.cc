#include "sdl_keyboard.hpp"

namespace charlie
{
	SDLKeyboard::SDLKeyboard() : m_currentFrame{}, m_lastFrame{}
	{
		for (int i = 0; i < SDL_Scancode::SDL_NUM_SCANCODES; i++)
		{
			m_lastFrame[i] = false;
			m_currentFrame[i] = false;
		}
	}

	void SDLKeyboard::Update()
	{
		for (int i = 0; i < SDL_Scancode::SDL_NUM_SCANCODES; i++)
		{
			m_lastFrame[i] = m_currentFrame[i];
		}
	}

	bool SDLKeyboard::IsKeyDown(SDL_Scancode p_key)
	{
		return m_currentFrame[p_key];
	}

	bool SDLKeyboard::IsKeyPressed(SDL_Scancode p_key)
	{
		return (m_lastFrame[p_key] == false && m_currentFrame[p_key] == true);
	}

	bool SDLKeyboard::IsKeyReleased(SDL_Scancode p_key)
	{
		return (m_lastFrame[p_key] == true && m_currentFrame[p_key] == false);
	}

	void SDLKeyboard::UpdateKey(SDL_Scancode p_key, bool p_value)
	{
		m_currentFrame[p_key] = p_value;
	}
}