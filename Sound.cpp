#include "Sound.h"
#include <iostream>
using namespace std;

Sound::Sound(void)
{
	audio_rate = 44100;
	audio_format = AUDIO_S16SYS;
	audio_channels = 1;
	audio_buffers = 4096;
	backgroundMusic = NULL;
	backgroundMusicChannel = 0;
	ballCollision = NULL;	
	instance = this;
}

void Sound::init(void) {
	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
		cerr << "Unable to initialize audio: " << Mix_GetError() << endl;
	}
	/**
	if(Mix_InitMP3() != 0) {
		cerr << "Unable to initialize mp3: " << Mix_GetError() << endl;
	}
	**/
	Mix_AllocateChannels(32);
	
	ballCollision = Mix_LoadWAV("resources/ball_collision.wav");
	if (ballCollision == NULL) {
		cout << "Unable to load file: " << Mix_GetError() << endl;
	}
	fireworksLaunch = Mix_LoadWAV("resources/fireworks_launch.wav");
	if (fireworksLaunch == NULL) {
		cout << "Unable to load file: " << Mix_GetError() << endl;
	}
	fireworksExplode = Mix_LoadWAV("resources/fireworks_Explode.wav");
	if (fireworksExplode == NULL) {
		cout << "Unable to load file: " << Mix_GetError() << endl;
	}
	missileLaunch = Mix_LoadWAV("resources/missile.wav");
	if (missileLaunch == NULL) {
		cout << "Unable to load file: " << Mix_GetError() << endl;
	}
	missilePickup = Mix_LoadWAV("resources/missile_pickup.wav");
	if (missilePickup == NULL) {
		cout << "Unable to load file: " << Mix_GetError() << endl;
	}
	fastballPickup = Mix_LoadWAV("resources/fastball.wav");
	if (fastballPickup == NULL) {
		cout << "Unable to load file: " << Mix_GetError() << endl;
	}
	missileExplode = Mix_LoadWAV("resources/explosion.wav");
	if (missileExplode == NULL) {
		cout << "Unable to load file: " << Mix_GetError() << endl;
	}
	grow = Mix_LoadWAV("resources/grow.wav");
	if (grow == NULL) {
		cout << "Unable to load file: " << Mix_GetError() << endl;
	}
	shrink = Mix_LoadWAV("resources/shrink.wav");
	if (shrink == NULL) {
		cout << "Unable to load file: " << Mix_GetError() << endl;
	}
	victory = Mix_LoadMUS("resources/victory.mid");
	if (victory == NULL) {
		cout << "Unable to load file: " << Mix_GetError() << endl;
	}
}


Sound::~Sound(void)
{
	// should free sounds here too: Mix_FreeChunk(sound);
	Mix_HaltChannel(-1);
	if (backgroundMusic) {
		Mix_FreeMusic(backgroundMusic);
		backgroundMusic = NULL;
	}

	Mix_FreeChunk(fireworksLaunch);
	Mix_FreeChunk(fireworksExplode);
	Mix_FreeChunk(missilePickup);
	Mix_FreeChunk(fastballPickup);
	Mix_FreeMusic(victory);
	Mix_CloseAudio();
}

int Sound::loopBackgroundMusic(char* fileName) {
	//I think this function loads every type of sound - TD 
	//if (backgroundMusic == NULL) {
		backgroundMusic = Mix_LoadMUS(fileName);
	//}
	if (Mix_PlayingMusic) {
		Mix_HaltMusic();
	}
	Mix_FadeInMusic(backgroundMusic, 0, 250);
	Mix_VolumeMusic(16);

	//Mix_Volume(backgroundMusicChannel, 64);
	
	return 0;
}


int Sound::stopBackgroundMusic() {
	Mix_FadeOutMusic(100);
	return 0;
}

void Sound::playSound(SoundType type) {
	int channel = 0;
	switch (type) {
		case BALL_COLLISION:
			channel = Mix_PlayChannel(-1, ballCollision, 0);
			break;
		case MISSILE_LAUNCH:
			channel = Mix_PlayChannel(-1, missileLaunch, 0);
			break;
		case MISSILE_EXPLODE:
			channel = Mix_PlayChannel(-1, missileExplode, 0);
			break;
		case GROW:
			channel = Mix_PlayChannel(-1, grow, 0);
			break;
		case SHRINK:
			channel = Mix_PlayChannel(-1, shrink, 0);
			break;
		case FIREWORKS_LAUNCH:
			channel = Mix_PlayChannel(-1, fireworksLaunch, 0);
			break;
		case FIREWORKS_EXPLODE:
			channel = Mix_PlayChannel(-1, fireworksExplode, 0);
			break;
		case VICTORY:
			channel = Mix_PlayMusic(victory, 0);
			Mix_VolumeMusic(32);
			break;
		case MISSILE_PICKUP:
			channel = Mix_PlayChannel(-1, missilePickup, 0);
			break;
		case FASTBALL_PICKUP:
			channel = Mix_PlayChannel(-1, fastballPickup, 0);
			break;
	}

	switch (type) {
		case FIREWORKS_LAUNCH:
			Mix_Volume(channel, 64);
			break;
		case FIREWORKS_EXPLODE:
			Mix_Volume(channel, 96);
			break;
		default:
			Mix_Volume(channel, 128);
			break;
	}
}

Sound* Sound::instance;

Sound* Sound::getInstance() {
	return instance;
}

int Sound::isMusicPlaying() {
	return Mix_PlayingMusic();
}
