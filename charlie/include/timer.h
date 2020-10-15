#pragma once
#include <SDL.h>

namespace charlie
{
	class Timer
	{
	public:
		Uint32 get_ticks() const;
		Uint32 get_seconds() const;
		Uint32 get_minutes() const;
		int get_time_left();
		void start();
	private:
		bool m_isStarted = false;
		bool m_isPaused = false;
		Uint32 m_pause = 0;
		Uint32 m_start = 0;
		Uint32 m_roundTime = 360; // TODO change to config
	};
}