#include "sdl_mouse.hpp"

namespace charlie
{
	SDLMouse::SDLMouse() : m_currentFrame{}, m_lastFrame{}
	{
		for (int i = 0; i < m_buttons; i++)
		{
			m_lastFrame[i] = 0;
			m_currentFrame[i] = 0;
		}
		m_currentWheelX = 0; m_lastWheelX = 0;
		m_currentWheelY = 0; m_lastWheelY = 0;
		m_currentX = 0;	m_lastX = 0;
		m_currentY = 0;	m_lastY = 0;
	}

	SDLMouse::~SDLMouse()
	{
	}

	void SDLMouse::Update()
	{
		for (int i = 0; i < m_buttons; i++)
		{
			m_lastFrame[i] = m_currentFrame[i];
		}
		m_lastX = m_currentX;
		m_lastY = m_currentY;
		m_lastWheelX = m_currentWheelX;
		m_lastWheelY = m_currentWheelY;
	}

	bool SDLMouse::IsButtonDown(unsigned int p_button)
	{
		if (p_button >= m_buttons)
			return false;

		return m_currentFrame[p_button];
	}

	bool SDLMouse::IsButtonPressed(unsigned int p_button)
	{
		if (p_button >= m_buttons)
			return false;

		return (m_lastFrame[p_button] == false && m_currentFrame[p_button] == true);
	}

	bool SDLMouse::IsButtonReleased(unsigned int p_button)
	{
		if (p_button >= m_buttons)
			return false;

		return (m_lastFrame[p_button] == true && m_currentFrame[p_button] == false);
	}

	void SDLMouse::UpdateButton(unsigned int p_button, bool p_value)
	{
		if (p_button >= m_buttons)
			return;

		m_currentFrame[p_button] = p_value;
	}

	void SDLMouse::UpdateWheel(int p_x, int p_y)
	{
		m_currentWheelX += p_x; m_currentWheelY += p_y;
	}

	void SDLMouse::UpdateMotion(int p_x, int p_y)
	{
		m_currentX = p_x; m_currentY = p_y;
	}

	int SDLMouse::GetMotionX()
	{
		return m_currentX - m_lastX;
	}

	int SDLMouse::GetMotionY()
	{
		return m_currentY - m_lastY;
	}

	int SDLMouse::GetPositionX()
	{
		return m_currentX;
	}

	int SDLMouse::GetPositionY()
	{
		return m_currentY;
	}

	int SDLMouse::GetWheelMotionX()
	{
		return m_currentWheelX - m_lastWheelX;
	}

	int SDLMouse::GetWheelMotionY()
	{
		return m_currentWheelY - m_lastWheelY;
	}

	int SDLMouse::GetWheelPositionX()
	{
		return m_currentWheelX;
	}

	int SDLMouse::GetWheelPositionY()
	{
		return m_currentWheelY;
	}
}