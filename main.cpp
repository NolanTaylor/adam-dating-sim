#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include "Sprites.h"

void typewrite(int x_center, int y, std::string text, bool type_writer_effect, float size = 1);
bool waitKey(), waitC();

int main(int argc, char* args[])
{
	window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);

	Sprite Adam; Adam.loadFromFile("Sprites/Adam.png");
	Sprite dialogue; dialogue.loadFromFile("Sprites/dialogue.png");
	Sprite bubble; bubble.loadFromFile("Sprites/SpeechBubble.png");
	Sprite heart; heart.loadFromFile("Sprites/Heart.png");
	Sprite egg; egg.loadFromFile("Sprites/egg.png");

	const int xpos = 150, ypos1 = 650 - 126 + 20,
			  ypos2 = 650 - 126 + 80;

	bool quit = false, button1 = true, button2 = true, speaking = false,
		 datingHUD = false;
	int act = 0, select = 0, count = 0,
		choice_x = SCREEN_WIDTH / 2,
		speechBubblex = 650,
		speechBubble1 = 20,
		speechBubble2 = 60,
		speechBubble3 = 100,
		speechBubble4 = 140,
		heart_x = xpos, heart_y = ypos1,
		choice_y1 = SCREEN_HEIGHT - dialogue.getHeight() + 20,
		choice_y2 = SCREEN_HEIGHT - dialogue.getHeight() + 80;

	Uint32 startTime = 0;
	Uint32 endTime = 0;
	Uint32 delta = 0;
	short fps = 60;
	short timePerFrame = 1000 / 60; // miliseconds

	SDL_Event event;

	int frame = 0;

	while (!quit)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
				quit = true;
			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_w:
					heart_y = ypos1;
					break;
				case SDLK_a:
					break;
				case SDLK_s:
					heart_y = ypos2;
					break;
				case SDLK_d:
					break;
				case SDLK_SPACE:
					switch (heart_y)
					{
					case ypos1:
						select = 1;
						break;
					case ypos2:
						select = 2;
						break;
					}
					break;
				}
			}
		}

		if (!startTime)
			startTime = SDL_GetTicks();
		else
			delta = endTime - startTime;

		if (delta < timePerFrame)
			SDL_Delay(timePerFrame - delta);
		if (delta > timePerFrame)
			fps = 1000 / delta;

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);

		/*----------------------------------------*/

		if (datingHUD)
		{
			break;
		}
		Adam.render((SCREEN_WIDTH - Adam.getWidth()) / 2, 0, NULL);
		dialogue.render((SCREEN_WIDTH - dialogue.getWidth()) / 2, SCREEN_HEIGHT - dialogue.getHeight() - 1, NULL);
		if (speaking)
			bubble.render(500, 10, NULL);

		switch (act)
		{
		case 0:
			typewrite(choice_x, choice_y1, "wasd to move", false, 2);
			typewrite(choice_x, choice_y2, "space to select advance", false, 2);
			heart.render(heart_x, heart_y, NULL);
			if (select)
				act++;
			break;
		case 1:
			speaking = true;
			act++;
			break;
		case 2:
			typewrite(speechBubblex, speechBubble1, "Okay", true, 2);
			typewrite(speechBubblex, speechBubble2, "Dating start", true, 2);
			if (!waitKey()) { quit = true; }
			speaking = false;
			act++; break;
		case 3:
			typewrite(choice_x, choice_y1, "DATING START", true, 4);
			if (!waitKey()) { quit = true; }
			act++; speaking = true; break;
		case 4:
			typewrite(speechBubblex, speechBubble1, "Here we are", true, 2);
			typewrite(speechBubblex, speechBubble2, "On our date", true, 2);
			if (!waitKey()) { quit = true; }
			act++; break;
		case 5:
			typewrite(speechBubblex, speechBubble1, "Ive actually", true, 2);
			typewrite(speechBubblex, speechBubble2, "never done this", true, 2);
			typewrite(speechBubblex, speechBubble3, "before", true, 2);
			if (!waitKey()) { quit = true; }
			act++; break;
		case 6:
			typewrite(speechBubblex, speechBubble1, "But dont worry", true, 2);
			if (!waitKey()) { quit = true; }
			act++; break;
		case 7:
			typewrite(speechBubblex, speechBubble1, "you cant spell", true, 2);
			typewrite(speechBubblex, speechBubble2, "prepared without", true, 2);
			typewrite(speechBubblex, speechBubble3, "several letters", true, 2);
			typewrite(speechBubblex, speechBubble4, "from my name", true, 2);
			if (!waitKey()) { quit = true; }
			act++; break;
		case 8:
			typewrite(speechBubblex, speechBubble1, "I snagged an", true, 2);
			typewrite(speechBubblex, speechBubble2, "official dating", true, 2);
			typewrite(speechBubblex, speechBubble3, "rulebook from", true, 2);
			typewrite(speechBubblex, speechBubble4, "the library", true, 2);
			if (!waitKey()) { quit = true; }
			act++; break;
		case 9:
			typewrite(speechBubblex, speechBubble1, "were ready to", true, 2);
			typewrite(speechBubblex, speechBubble2, "have a great", true, 2);
			typewrite(speechBubblex, speechBubble3, "time", true, 2);
			if (!waitKey()) { quit = true; }
			act++; break;
		case 10:
			typewrite(speechBubblex, speechBubble1, "Lets see", true, 2);
			if (!waitKey()) { quit = true; }
			act++; break;
		case 11:
			typewrite(speechBubblex, speechBubble1, "Step one Press", true, 2);
			typewrite(speechBubblex, speechBubble2, "the c key on", true, 2);
			typewrite(speechBubblex, speechBubble3, "your keyboard", true, 2);
			typewrite(speechBubblex, speechBubble4, "for dating hud", true, 2);
			if (!waitC()) { quit = true; }
			act++; break;
		case 12:
			//bring up dating hud (function?)
			typewrite(speechBubblex, speechBubble1, "Wowie", true, 2); SDL_Delay(250);
			typewrite(speechBubblex, speechBubble2, "I feel so", true, 2);
			typewrite(speechBubblex, speechBubble3, "informed", true, 2);
			if (!waitKey()) { quit = true; }
			act++; break;
		default:
			break;
		}

		/*----------------------------------------*/

		SDL_RenderPresent(renderer);

		startTime = endTime;
		endTime = SDL_GetTicks();
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	IMG_Quit();
	SDL_Quit();

	return 0;
}

void typewrite(int x_center, int y, std::string text, bool type_writer_effect, float size)
{
	Sprite words; words.loadFromFile("Sprites/text.png");
	Sound textSound; textSound.loadFromFile("Audio/snd_txtpap.wav");
	SDL_Rect character[63];

	for (int i = 0; i < 63; i++)
	{
		character[i].x = ((i % 9) * 11);
		character[i].y = ((i / 9) * 14);
		character[i].w = 10;
		character[i].h = 13;
	}

	textSound.play(text.size());

	for (std::string::size_type i = 0; i < text.size(); i++)
	{
		int letter = static_cast<int>(text[i]); // -er mage

		if (letter <= 90)
			letter -= 65;
		else if (letter >= 97)
			letter -= 71;

		if (size == 1) // oh dear god end me now
			words.render((x_center - (text.size() * 8) / 2) + i * 8, y, &character[letter]);
		else
			words.renderScaled((x_center - (text.size() * 8 * size) / 2) + i * 8 * size, y, size, &character[letter]);

		if (type_writer_effect)
		{
			SDL_RenderPresent(renderer);
			SDL_Delay(50);
		}
	}
}

bool waitKey()
{
	SDL_Event event;

	while (true)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
				return false;
			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_SPACE:
					return true;
					break;
				}
			}
		}
	}
}

bool waitC()
{
	SDL_Event event;

	while (true)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
				return false;
			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_c:
					return true;
					break;
				}
			}
		}
	}
}