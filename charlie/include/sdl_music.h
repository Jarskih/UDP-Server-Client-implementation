#pragma once
#include <SDL_mixer.h>

namespace charlie
{
	class Music
	{
		Mix_Music* m_music;
		Music(Mix_Music* p_chunk);
		~Music();
		void Play(int p_loops) const;
		static void Pause();
		static void Resume();
	};
}
