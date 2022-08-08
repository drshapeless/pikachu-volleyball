#include "sprite.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

SDL_Texture *NewPikaSpriteSheetTexture(SDL_Renderer *renderer)
{
	/* Load image. */
	SDL_Surface *surface = IMG_Load("assets/images/sprite_sheet.png");
	if (surface == NULL) {
		printf("Error loading image: %s\n", IMG_GetError());
		return NULL;
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == NULL) {
		printf("Texture could not be created! SDL_Error: %s\n",
		       SDL_GetError());
		return NULL;
	}
	SDL_FreeSurface(surface);

	return texture;
}

const SDL_Rect *PikachuRect(int state, int frame)
{
	switch (state) {
	case 0:
		if (frame > 4)
			break;
		return &PikaTextureRect[pikachu_0_0 + frame];
	case 1:
		if (frame > 4)
			break;
		return &PikaTextureRect[pikachu_1_0 + frame];
	case 2:
		if (frame > 4)
			break;
		return &PikaTextureRect[pikachu_2_0 + frame];
	case 3:
		if (frame > 1)
			break;
		return &PikaTextureRect[pikachu_3_0 + frame];
	case 4:
		if (frame > 0)
			break;
		return &PikaTextureRect[pikachu_4_0];
	case 5:
		if (frame > 4)
			break;
		return &PikaTextureRect[pikachu_5_0 + frame];
	case 6:
		if (frame > 4)
			break;
		return &PikaTextureRect[pikachu_6_0 + frame];
	default:
		break;
	}

	return NULL;
}

const SDL_Rect *NumberRect(int i)
{
	if (i > 9) {
		return NULL;
	}

	return &PikaTextureRect[number_0 + i];
}

const SDL_Rect *BallRect(int rotation)
{
	if (rotation > 5) {
		return NULL;
	}

	return &PikaTextureRect[ball_0 + rotation];
}
