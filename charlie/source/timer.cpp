#include "timer.h"

namespace charlie
{
	Uint32 Timer::get_ticks() const
	{
		return SDL_GetTicks() - m_start;
	}

	Uint32 Timer::get_seconds() const
	{
		if (!m_isPaused)
		{
			return m_roundTime - get_ticks() / 1000 - get_minutes() * 60;
		}
		return 0;
	}

	Uint32 Timer::get_minutes() const
	{
		if (!m_isPaused)
		{
			return (m_roundTime - get_ticks() / 1000) / 60;
		}
		return 0;
	}

	int Timer::get_time_left()
	{
		const int timeLeft = (int)(m_roundTime - get_ticks() / 1000.0f);
		if (timeLeft <= 0)
		{
			m_isPaused = true;
		}
		return timeLeft;
	}

	void Timer::start()
	{
		m_isStarted = true;
		m_isPaused = false;
		m_start = SDL_GetTicks();
	}
}
