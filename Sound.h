#pragma once
#include <SDL/SDL_mixer.h>

enum SoundType { BALL_COLLISION, MISSILE_LAUNCH, 
MISSILE_EXPLODE, GROW, SHRINK, FIREWORKS_LAUNCH, 
FIREWORKS_EXPLODE, VICTORY, MISSILE_PICKUP, FASTBALL_PICKUP };

class Sound
{
	public:
		Sound(void);
		virtual ~Sound(void);
		void init(void);
		int loopBackgroundMusic(char* fileName);
		int stopBackgroundMusic();
		void playSound(SoundType type);
		static Sound* getInstance();
		int isMusicPlaying();

	private:
		static Sound* instance;
		int audio_rate;
		Uint16 audio_format;
		int audio_channels;
		int audio_buffers;
		Mix_Music* backgroundMusic;
		int backgroundMusicChannel;

		Mix_Chunk* ballCollision;
		Mix_Chunk* missileLaunch;
		Mix_Chunk* missileExplode;
		Mix_Chunk* missilePickup;
		Mix_Chunk* fastballPickup;
		Mix_Chunk* grow;
		Mix_Chunk* shrink;
		Mix_Chunk* fireworksLaunch;
		Mix_Chunk* fireworksExplode;
		Mix_Music* victory;
};


