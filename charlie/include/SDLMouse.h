#pragma once

struct SDLMouse
{
	static const unsigned int m_buttons = 5;
	bool m_currentFrame[m_buttons];
	bool m_lastFrame[m_buttons];
	int m_currentWheelX;
	int m_lastWheelX;
	int m_currentWheelY;
	int m_lastWheelY;
	int m_currentX;
	int m_lastX;
	int m_currentY;
	int m_lastY;

	SDLMouse();
	~SDLMouse();
	void Update();
	bool IsButtonDown(unsigned int p_button);
	bool IsButtonPressed(unsigned int p_button);
	bool IsButtonReleased(unsigned int p_button);
	void UpdateButton(unsigned int p_button, bool p_value);
	void UpdateWheel(int p_x, int p_y);
	void UpdateMotion(int p_x, int p_y);
	int  GetMotionX();
	int  GetMotionY();
	int  GetPositionX();
	int  GetPositionY();
	int  GetWheelMotionX();
	int  GetWheelMotionY();
	int  GetWheelPositionX();
	int  GetWheelPositionY();
};
