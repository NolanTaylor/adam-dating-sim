#pragma once

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 700;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

class Sprite
{
public:
	Sprite(std::string path) { mTexture = NULL; w = 0; h = 0; loadFromFile(path); }

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

	void render(int x, int y, SDL_Rect* clip = NULL, float scalar = 1)
	{
		SDL_Rect renderQuad = { x, y, w * scalar, h * scalar };

		if (clip != NULL)
		{
			renderQuad.w = clip->w * scalar;
			renderQuad.h = clip->h * scalar;
		}

		SDL_RenderCopy(renderer, mTexture, clip, &renderQuad);
	}

	void renderEx(int x, int y, SDL_Rect* clip = NULL, float scalar = 1, double angle = NULL,
		SDL_Point * center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE)
	{
		SDL_Rect renderQuad = { x, y, w * scalar, h * scalar };

		if (clip != NULL)
		{
			renderQuad.w = clip->w * scalar;
			renderQuad.h = clip->h * scalar;
		}

		SDL_RenderCopyEx(renderer, mTexture, clip, &renderQuad, angle, center, flip);
	}

	int getWidth() { return w; }
	int getHeight() { return h; }

	SDL_Texture* mTexture;

	int w, h;
	bool beta = 1, underneath = false, underneath_again = false;
};

class Text
{
public:
	Text() // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA!
	{
		spriteFree();

		SDL_Texture* newTexture = NULL;
		SDL_Surface* loadedSurface = IMG_Load("Sprites/text.png");

		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);

		w = loadedSurface->w;
		h = loadedSurface->h;

		SDL_FreeSurface(loadedSurface);

		mTexture = newTexture;

		for (int i = 0; i < 63; i++)
		{
			character[i].x = ((i % 9) * 11);
			character[i].y = ((i / 9) * 14);
			character[i].w = 10;
			character[i].h = 13;
		}
	}

	~Text() { spriteFree(); audioFree(); }

	bool type(SDL_Rect renderArea, bool typing_effect = false, float size = 1)
	{
		int j = 0;
		if (typing_effect)
		{
			if (pos == 1)
				play(text.size());
			for (int i = 0; i < pos / 8; i++)
			{
				render(renderArea.x + (j * 8 * size), renderArea.y, &character[getLetter(text[i])], size);
				if (text[i] == '/')
				{
					renderArea.y += 30;
					j = -1;
				} j++;
			}

			pos++;

			if (pos > text.size() * 8)
			{
				pos = 0;
				text = "";
				stop();
				return true;
			}
			else
				return false;
		}
		else
		{
			for (int i = 0; i < text.size(); i++)
			{
				render(renderArea.x + (j * 8 * size), renderArea.y, &character[getLetter(text[i])], size);
				if (text[i] == '/')
				{
					renderArea.y += 30;
					j = -1;
				} j++;
			}
			return true;
		}
	}

	void setString(std::string string)
	{
		text = string;
	}

	bool getTyping()
	{
		if (text.empty()) { return false; }
		else { return true; }
	}

private:
	void render(int x, int y, SDL_Rect* clip = NULL, float scalar = 1)
	{
		SDL_Rect renderQuad = { x, y, w * scalar, h * scalar };

		if (clip != NULL)
		{
			renderQuad.w = clip->w * scalar;
			renderQuad.h = clip->h * scalar;
		}

		SDL_RenderCopy(renderer, mTexture, clip, &renderQuad);
	}

	int getLetter(int index)
	{
		int letter = static_cast<int>(index); // -er mage

		if (letter >= 32 && letter < 65)
			letter += 20;
		else if (letter >= 65 && letter <= 90)
			letter -= 65;
		else if (letter >= 97)
			letter -= 71;

		return letter;
	}

	void play(int repetitions)
	{
		Mix_PlayChannel(5, audio, repetitions);
	}
	
	void stop()
	{
		Mix_HaltChannel(5);
	}

	void spriteFree()
	{
		if (mTexture != NULL)
		{
			SDL_DestroyTexture(mTexture);
			mTexture = NULL;
			w = 0;
			h = 0;
		}
	}

	void audioFree()
	{
		Mix_FreeChunk(audio);
	}

	SDL_Texture* mTexture;
	SDL_Rect character[63];
	Mix_Chunk* audio = Mix_LoadWAV("Audio/textSound.wav");

	int w, h, pos = 0;
	std::string text;
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

	void stop()
	{
		Mix_HaltChannel(-1);
	}

	void free()
	{
		Mix_FreeChunk(audio);
	}
};