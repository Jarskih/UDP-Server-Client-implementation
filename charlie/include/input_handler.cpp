#include "input_handler.h"
#include <SDL.h>

bool InputHandler::IsKeyDown(SDL_Scancode p_key)
{
	return m_keyboard.IsKeyDown(p_key);
}

bool InputHandler::IsKeyPressed(SDL_Scancode p_key)
{
	return m_keyboard.IsKeyPressed(p_key);
}

bool InputHandler::IsKeyReleased(SDL_Scancode p_key)
{
	return m_keyboard.IsKeyReleased(p_key);
}

bool InputHandler::IsMouseButtonDown(unsigned int p_button)
{
	return m_mouse.IsButtonDown(p_button);
}

bool InputHandler::IsMouseButtonPressed(unsigned int p_button)
{
	return m_mouse.IsButtonPressed(p_button);
}

bool InputHandler::IsMouseButtonReleased(unsigned int p_button)
{
	return m_mouse.IsButtonReleased(p_button);
}

int InputHandler::GetMouseMotionX()
{
	return m_mouse.GetMotionX();
}

int InputHandler::GetMouseMotionY()
{
	return m_mouse.GetMotionY();
}

int InputHandler::GetMousePositionX()
{
	return m_mouse.GetPositionX();
}

int InputHandler::GetMousePositionY()
{
	return m_mouse.GetPositionY();
}

int InputHandler::GetMouseWheelMotionX()
{
	return m_mouse.GetWheelMotionX();
}

int InputHandler::GetMouseWheelMotionY()
{
	return m_mouse.GetWheelMotionY();
}

int InputHandler::GetMouseWheelPositionX()
{
	return m_mouse.GetWheelPositionX();
}

int InputHandler::GetMouseWheelPositionY()
{
	return m_mouse.GetWheelPositionY();
}

void InputHandler::HandleEvents()
{
	m_keyboard.Update();
	m_mouse.Update();

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_EventType::SDL_KEYDOWN: m_keyboard.UpdateKey(event.key.keysym.scancode, true); break;
		case SDL_EventType::SDL_KEYUP: m_keyboard.UpdateKey(event.key.keysym.scancode, false); break;
		case SDL_EventType::SDL_MOUSEBUTTONDOWN: m_mouse.UpdateButton(event.button.button, true); break;
		case SDL_EventType::SDL_MOUSEBUTTONUP: m_mouse.UpdateButton(event.button.button, false); break;
		case SDL_EventType::SDL_MOUSEWHEEL: m_mouse.UpdateWheel(event.wheel.x, event.wheel.y); break;
		case SDL_EventType::SDL_MOUSEMOTION: m_mouse.UpdateMotion(event.motion.x, event.motion.y); break;
		}
	}
}
