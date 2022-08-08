#include "sound.h"

#include <stdlib.h>

struct PikaAudio *NewPikaAudio(void)
{
	Mix_Music *bgm = Mix_LoadMUS("../assets/sounds/bgm.wav");
	if (bgm == NULL) {
		printf("Failed to load beat music! SDL_mixer Error: %s\n",
		       Mix_GetError());
		return NULL;
	}

	Mix_Chunk *pipikachu = Mix_LoadWAV("../assets/sounds/WAVE140_1.wav");
	if (pipikachu == NULL) {
		printf("Failed to load pipikachu sound effect! SDL_mixer Error: %s\n",
		       Mix_GetError());
		return NULL;
	}

	Mix_Chunk *pika = Mix_LoadWAV("../assets/sounds/WAVE141_1.wav");
	if (pika == NULL) {
		printf("Failed to load pika sound effect! SDL_mixer Error: %s\n",
		       Mix_GetError());
		return NULL;
	}

	Mix_Chunk *chu = Mix_LoadWAV("../assets/sounds/WAVE142_1.wav");
	if (chu == NULL) {
		printf("Failed to load chu sound effect! SDL_mixer Error: %s\n",
		       Mix_GetError());
		return NULL;
	}

	Mix_Chunk *pi = Mix_LoadWAV("../assets/sounds/WAVE143_1.wav");
	if (pi == NULL) {
		printf("Failed to load pi sound effect! SDL_mixer Error: %s\n",
		       Mix_GetError());
		return NULL;
	}

	Mix_Chunk *pikachu = Mix_LoadWAV("../assets/sounds/WAVE144_1.wav");
	if (pi == NULL) {
		printf("Failed to load pikachu sound effect! SDL_mixer Error: %s\n",
		       Mix_GetError());
		return NULL;
	}
	Mix_Chunk *powerhit = Mix_LoadWAV("../assets/sounds/WAVE145_1.wav");
	if (powerhit == NULL) {
		printf("Failed to load powerhit sound effect! SDL_mixer Error: %s\n",
		       Mix_GetError());
		return NULL;
	}

	Mix_Chunk *ballTouchesGround =
		Mix_LoadWAV("../assets/sounds/WAVE146_1.wav");
	if (ballTouchesGround == NULL) {
		printf("Failed to load ballTouchesGround sound effect! SDL_mixer Error: %s\n",
		       Mix_GetError());
	}

	struct PikaAudio *audio = malloc(sizeof(struct PikaAudio));
	audio->bgm = bgm;
	audio->pipikachu = pipikachu;
	audio->pika = pika;
	audio->chu = chu;
	audio->pi = pi;
	audio->pikachu = pikachu;
	audio->powerhit = powerhit;
	audio->ballTouchesGround = ballTouchesGround;

	return audio;
}

void DestroyPikaAudio(struct PikaAudio *audio)
{
	Mix_FreeMusic(audio->bgm);
	Mix_FreeChunk(audio->pipikachu);
	Mix_FreeChunk(audio->pika);
	Mix_FreeChunk(audio->chu);
	Mix_FreeChunk(audio->pi);
	Mix_FreeChunk(audio->pikachu);
	Mix_FreeChunk(audio->powerhit);
	Mix_FreeChunk(audio->ballTouchesGround);

	free(audio);
}
