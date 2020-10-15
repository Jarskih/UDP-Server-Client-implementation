#include "sdl_music.h"

namespace charlie
{
	Music::Music(Mix_Music* p_music)
	{
		m_music = p_music;
	}

	Music::~Music()
	{
		m_music = nullptr;
	}

	void Music::Play(int p_loops) const
	{
		Mix_PlayMusic(m_music, p_loops);
	}

	void Music::Pause()
	{
		Mix_PauseMusic();
	}

	void Music::Resume()
	{
		Mix_ResumeMusic();
	}
}