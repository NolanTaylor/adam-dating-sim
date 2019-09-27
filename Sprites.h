#pragma once

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 650;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

class Sprite
{
public:
	Sprite() { mTexture = NULL; w = 0; h = 0; }

	~Sprite() { free(); }

	bool loadFromFile(std::string path)
	{
		free();

		SDL_Texture* newTexture = NULL;
		SDL_Surface* loadedSurface = IMG_Load(path.c_str());

		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);

		w = loadedSurface->w;
		h = loadedSurface->h;

		SDL_FreeSurface(loadedSurface);

		mTexture = newTexture;
		return mTexture != NULL;
	}

	void setAlpha(Uint8 alpha)
	{
		SDL_SetTextureAlphaMod(mTexture, alpha);
		if (alpha == 255) { beta = 1; }
		else { beta = 0; }
	}

	void swapAlpha()
	{
		if (underneath != underneath_again)
		{
			if (beta) { setAlpha(127); }
			else { setAlpha(255); }
		}
	}

	void free()
	{
		if (mTexture != NULL)
		{
			SDL_DestroyTexture(mTexture);
			mTexture = NULL;
			w = 0;
			h = 0;
		}
	}

	void render(int x, int y, SDL_Rect* clip = NULL)
	{
		SDL_Rect renderQuad = { x, y, w, h };

		if (clip != NULL)
		{
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}

		SDL_RenderCopy(renderer, mTexture, clip, &renderQuad);
	}

	void renderScaled(int x, int y, float scalar = 1, SDL_Rect * clip = NULL)
	{
		SDL_Rect renderQuad = { x, y, w * scalar, h * scalar };

		if (clip != NULL)
		{
			renderQuad.w = clip->w * scalar;
			renderQuad.h = clip->h * scalar;
		}

		SDL_RenderCopy(renderer, mTexture, clip, &renderQuad);
	}

	int getWidth() { return w; }
	int getHeight() { return h; }

	SDL_Texture* mTexture;

	int w, h;
	bool beta = 1, underneath = false, underneath_again = false;
};

class Sound
{
public:
	Sound() { audio = NULL; };

	~Sound() { free(); }

	Mix_Chunk* audio;

	void loadFromFile(std::string path)
	{
		audio = Mix_LoadWAV(path.c_str());
	}

	void play(int repetitions)
	{
		Mix_PlayChannel(-1, audio, repetitions);
	}

	void free()
	{
		Mix_FreeChunk(audio);
	}
};