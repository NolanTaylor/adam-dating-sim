#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include "Sprites.h"

void typewrite(int x_center, int y, std::string text, bool type_writer_effect, float size = 1);
int threadFunction(void* data);

bool quit = false, datingHUD_cont = false;

SDL_sem* dataLock = false; // semaphore

SDL_Rect graphRect = { 0, 0, 120, 120 };

int main(int argc, char* args[])
{
	window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	dataLock = SDL_CreateSemaphore(1);

	Sprite Adam; Adam.loadFromFile("Sprites/Adam.png");
	Sprite dialogue; dialogue.loadFromFile("Sprites/dialogue.png");
	Sprite bubble; bubble.loadFromFile("Sprites/SpeechBubble.png");
	Sprite heart; heart.loadFromFile("Sprites/Heart.png");
	Sprite egg; egg.loadFromFile("Sprites/egg.png");
	Sprite graphFrame; graphFrame.loadFromFile("Sprites/graphFrame.png");
	Sprite graph; graph.loadFromFile("Sprites/graph.png");

	Sound DatingStart; DatingStart.loadFromFile("Audio/DatingStart.wav");
	Sound DatingTense; DatingTense.loadFromFile("Audio/DatingTense.wav");
	Sound DatingFight; DatingFight.loadFromFile("Audio/DatingFight.wav");

	const int xpos = 150, ypos1 = 650 - 126 + 20,
			  ypos2 = 650 - 126 + 80;

	bool button1 = true, button2 = true, speaking = false,
		 speech = true, select_c = false, datingHUD = false;
	int act = 52, select = 0, count = 0,
		choice_x = SCREEN_WIDTH / 2,
		speechBubblex = 660,
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
			{
				SDL_SemWait(dataLock);
				quit = true;
				SDL_SemPost(dataLock);
			}
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
				case SDLK_c:
					select_c = true;
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

		SDL_SetRenderDrawColor(renderer, 20, 20, 50, 255);
		SDL_RenderClear(renderer);

		/*----------------------------------------*/

		Adam.render((SCREEN_WIDTH - Adam.getWidth()) / 2, 0, NULL);
		dialogue.render((SCREEN_WIDTH - dialogue.getWidth()) / 2, SCREEN_HEIGHT - dialogue.getHeight() - 1, NULL);
		if (datingHUD)
		{
			SDL_Thread* threadID = SDL_CreateThread(threadFunction, "graphThingie", (void*)quit);
			datingHUD = false;
			datingHUD_cont = true;
		}
		else if (datingHUD_cont)
		{
			typewrite(60, 440, "egg", false);
			egg.render(50, 450, NULL);
			graph.render(70, 50, &graphRect);
			graphFrame.render(70, 50, NULL);
			if (graphRect.x >= 360) { graphRect.x = 0; }
			graphRect.x++;
		}
		if (speaking)
			bubble.render(500, 10, NULL);

		switch (act)
		{
		case 0:
			typewrite(choice_x, choice_y1, "wasd to move", false, 2);
			typewrite(choice_x, choice_y2, "space to select advance", false, 2);
			heart.render(heart_x, heart_y, NULL);
			if (select) { act++; select = 0; speaking = true; }
			break;
		case 1:
			typewrite(speechBubblex, speechBubble1, "So um", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 2:
			typewrite(speechBubblex, speechBubble1, "If youve seen", speech, 2);
			typewrite(speechBubblex, speechBubble2, "everything", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 3:
			typewrite(speechBubblex, speechBubble1, "Do you want to", speech, 2);
			typewrite(speechBubblex, speechBubble2, "start the date", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 4:
			typewrite(speechBubblex, speechBubble1, "Do you want to", false, 2);
			typewrite(speechBubblex, speechBubble2, "start the date", false, 2);
			typewrite(choice_x, choice_y1, "Start the date", false, 2);
			if (select) { select = 0; act++; }
			break;
		case 5:
			speaking = true;
			act++;
			break;
		case 6:
			typewrite(speechBubblex, speechBubble1, "Okay", speech, 2);
			typewrite(speechBubblex, speechBubble2, "Dating start", speech, 2);
			speech = false;
			if (select) { speech = true; speaking = false; select = 0; act++; }
			break;
		case 7:
			typewrite(choice_x, choice_y1, "DATING START", false, 4);
			if (select) { speaking = true; select = 0; DatingStart.play(1); act++; }
			break;
		case 8:
			typewrite(speechBubblex, speechBubble1, "Here we are", speech, 2);
			typewrite(speechBubblex, speechBubble2, "On our date", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 9:
			typewrite(speechBubblex, speechBubble1, "Ive actually", speech, 2);
			typewrite(speechBubblex, speechBubble2, "never done this", speech, 2);
			typewrite(speechBubblex, speechBubble3, "before", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 10:
			typewrite(speechBubblex, speechBubble1, "But dont worry", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 11:
			typewrite(speechBubblex, speechBubble1, "you cant spell", speech, 2);
			typewrite(speechBubblex, speechBubble2, "prepared without", speech, 2);
			typewrite(speechBubblex, speechBubble3, "several letters", speech, 2);
			typewrite(speechBubblex, speechBubble4, "from my name", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 12:
			typewrite(speechBubblex, speechBubble1, "I snagged an", speech, 2);
			typewrite(speechBubblex, speechBubble2, "official dating", speech, 2);
			typewrite(speechBubblex, speechBubble3, "rulebook from", speech, 2);
			typewrite(speechBubblex, speechBubble4, "the library", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 13:
			typewrite(speechBubblex, speechBubble1, "were ready to", speech, 2);
			typewrite(speechBubblex, speechBubble2, "have a great", speech, 2);
			typewrite(speechBubblex, speechBubble3, "time", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 14:
			typewrite(speechBubblex, speechBubble1, "Lets see", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 15:
			typewrite(speechBubblex, speechBubble1, "Step one Press", speech, 2);
			typewrite(speechBubblex, speechBubble2, "the c key on", speech, 2);
			typewrite(speechBubblex, speechBubble3, "your keyboard", speech, 2);
			typewrite(speechBubblex, speechBubble4, "for dating hud", speech, 2);
			speech = false;
			if (select_c)
			{
				speech = true;
				SDL_SemWait(dataLock);
				datingHUD = true;
				SDL_SemPost(dataLock);
				select_c = false;
				act++;
			}
			break;
		case 16:
			typewrite(speechBubblex, speechBubble1, "Wowie", speech, 2);
			if (speech) { SDL_Delay(250); }
			typewrite(speechBubblex, speechBubble2, "I feel so", speech, 2);
			typewrite(speechBubblex, speechBubble3, "informed", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 17:
			typewrite(speechBubblex, speechBubble1, "I think were", speech, 2);
			typewrite(speechBubblex, speechBubble2, "ready for", speech, 2);
			typewrite(speechBubblex, speechBubble3, "step 2", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 18:
			typewrite(speechBubblex, speechBubble1, "Step 2 ask", speech, 2);
			typewrite(speechBubblex, speechBubble2, "them on a date", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 19:
			typewrite(speechBubblex, speechBubble1, "Ahem", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 20:
			typewrite(speechBubblex, speechBubble1, "Human", speech, 2);
			typewrite(speechBubblex, speechBubble2, "I the great Adam", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 21:
			typewrite(speechBubblex, speechBubble1, "Will go on a", speech, 2);
			typewrite(speechBubblex, speechBubble2, "date with you", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 22:
			typewrite(choice_x, choice_y1, "yes", false, 2);
			typewrite(choice_x, choice_y2, "no", false, 2);
			heart.render(heart_x, heart_y, NULL);
			if (select == 1) { act = 123; }
			else if (select == 2) { act = 223; }
			speech = true; select = 0; break;
		case 123: // yes choice branch
			typewrite(speechBubblex, speechBubble1, "R Really", speech, 2);
			typewrite(speechBubblex, speechBubble2, "Wowie", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act = 24; }
			break;
		case 223: // no choice branch
			typewrite(speechBubblex, speechBubble1, "Fortunately it", speech, 2);
			typewrite(speechBubblex, speechBubble2, "only says to", speech, 2);
			typewrite(speechBubblex, speechBubble3, "ask", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act = 24; }
			break;
		case 24:
			typewrite(speechBubblex, speechBubble1, "I guess that", speech, 2);
			typewrite(speechBubblex, speechBubble2, "means its time", speech, 2);
			typewrite(speechBubblex, speechBubble3, "for part three", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 25:
			typewrite(speechBubblex, speechBubble1, "Step three", speech, 2);
			typewrite(speechBubblex, speechBubble2, "put on nice", speech, 2);
			typewrite(speechBubblex, speechBubble3, "clothes to", speech, 2);
			typewrite(speechBubblex, speechBubble4, "show you care", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 26:
			typewrite(speechBubblex, speechBubble1, "dot dot dot", speech, 2); //...
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 27:
			DatingStart.stop(); //music fade out
			typewrite(speechBubblex, speechBubble1, "Wait a second", speech, 2); //period
			speech = false;
			if (select) { speech = true; select = 0; DatingTense.play(1); act++; }
			break;
		case 28:
			typewrite(speechBubblex, speechBubble1, "Wear clothing", speech, 2); //quotations
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 29:
			typewrite(speechBubblex, speechBubble1, "Youre wearing", speech, 2);
			typewrite(speechBubblex, speechBubble2, "clothing right", speech, 2);
			typewrite(speechBubblex, speechBubble3, "now", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 30:
			typewrite(speechBubblex, speechBubble1, "Not only that", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 31:
			typewrite(speechBubblex, speechBubble1, "Earlier today", speech, 2);
			typewrite(speechBubblex, speechBubble2, "you were also", speech, 2);
			typewrite(speechBubblex, speechBubble3, "wearing clothing", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 32:
			typewrite(speechBubblex, speechBubble1, "No", speech, 2);
			typewrite(speechBubblex, speechBubble2, "could it be", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 33:
			typewrite(speechBubblex, speechBubble1, "Youve wanted", speech, 2);
			typewrite(speechBubblex, speechBubble2, "to date me from", speech, 2);
			typewrite(speechBubblex, speechBubble3, "the very", speech, 2);
			typewrite(speechBubblex, speechBubble4, "beginning", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 34:
			typewrite(choice_x, choice_y1, "yes", false, 2);
			typewrite(choice_x, choice_y2, "no", false, 2);
			heart.render(heart_x, heart_y, NULL);
			if (select == 1) { act = 134; DatingTense.stop(); }
			else if (select == 2) { act = 234; DatingTense.stop(); }
			speech = true; select = 0; break;
		case 134: // yes choice branch
			typewrite(speechBubblex, speechBubble1, "No!!!", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 135:
			typewrite(speechBubblex, speechBubble1, "You planned", speech, 2);
			typewrite(speechBubblex, speechBubble2, "it all", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 136:
			typewrite(speechBubblex, speechBubble1, "Youre way better", speech, 2);
			typewrite(speechBubblex, speechBubble2, "at dating than", speech, 2);
			typewrite(speechBubblex, speechBubble3, "I am", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act = 37; }
			break;
		case 234: // no choice branch Music stop datingHUD away Shake
			DatingTense.stop();
			typewrite(speechBubblex, speechBubble1, "Despite that you", speech, 2);
			typewrite(speechBubblex, speechBubble2, "chose to wear", speech, 2);
			typewrite(speechBubblex, speechBubble3, "clothing today", speech, 2);
			typewrite(speechBubblex, speechBubble4, "of all days", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 235:
			typewrite(speechBubblex, speechBubble1, "was your interest", speech, 2);
			typewrite(speechBubblex, speechBubble2, "in me", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 236:
			typewrite(speechBubblex, speechBubble1, "Predestined", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act = 37; }
			break;
		case 37:
			typewrite(speechBubblex, speechBubble1, "N-NOOOO!!!!", speech, 2);
			typewrite(speechBubblex, speechBubble2, "Your dating", speech, 2);
			typewrite(speechBubblex, speechBubble3, "power!!!", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 38:
			act++;
			break; // dating power?
		case 39:
			typewrite(speechBubblex, speechBubble1, "Nyeh", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 40:
			typewrite(speechBubblex, speechBubble1, "Nyeh heh heh", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; DatingFight.play(1); act++; }
			break;
		case 41: // DatingFight tension graph and music
			typewrite(speechBubblex, speechBubble1, "Dont think youve", speech, 2);
			typewrite(speechBubblex, speechBubble2, "bested me yet", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 42:
			typewrite(speechBubblex, speechBubble1, "I the great Adam", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 43:
			typewrite(speechBubblex, speechBubble1, "Have never been", speech, 2);
			typewrite(speechBubblex, speechBubble2, "beaten at dating", speech, 2);
			typewrite(speechBubblex, speechBubble3, "and I never will", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 44:
			typewrite(speechBubblex, speechBubble1, "I can easily", speech, 2);
			typewrite(speechBubblex, speechBubble2, "keep up with you", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 45:
			typewrite(speechBubblex, speechBubble1, "You see", speech, 2);
			typewrite(speechBubblex, speechBubble2, "I too can wear", speech, 2);
			typewrite(speechBubblex, speechBubble3, "clothing", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 46:
			typewrite(speechBubblex, speechBubble1, "In fact", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 47:
			typewrite(speechBubblex, speechBubble1, "I always wear my", speech, 2);
			typewrite(speechBubblex, speechBubble2, "SPECIAL CLOTHES", speech, 2);
			typewrite(speechBubblex, speechBubble3, "underneath my", speech, 2);
			typewrite(speechBubblex, speechBubble4, "regular clothes", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 48:
			typewrite(speechBubblex, speechBubble1, "Just in case", speech, 2);
			typewrite(speechBubblex, speechBubble2, "somebody happens", speech, 2);
			typewrite(speechBubblex, speechBubble3, "to ask me on a", speech, 2);
			typewrite(speechBubblex, speechBubble4, "date", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 49:
			typewrite(speechBubblex, speechBubble1, "Behold!", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 50:
			act++;
			break; // ADAM MOVE OUT!!!
		case 51:
			typewrite(speechBubblex, speechBubble1, "Nyeh", speech, 2);
			typewrite(speechBubblex, speechBubble2, "what do you", speech, 2);
			typewrite(speechBubblex, speechBubble3, "think of my", speech, 2);
			typewrite(speechBubblex, speechBubble4, "secret style", speech, 2);
			speech = false;
			if (select) { speaking = false; speech = true; select = 0; act++; }
			break;
		case 52:
			typewrite(choice_x, choice_y1, "I love it", false, 2);
			typewrite(choice_x, choice_y2, "I hate it", false, 2);
			heart.render(heart_x, heart_y, NULL);
			if (select == 1) { act = 153; }
			else if (select == 2) { act = 253; }
			speech = true; select = 0;
			break;
		case 153: // i love it choice branch
			speaking = true;
			typewrite(speechBubblex, speechBubble1, "NO!!!", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 154:
			typewrite(speechBubblex, speechBubble1, "A genuine", speech, 2);
			typewrite(speechBubblex, speechBubble2, "compliment", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act = 55; }
			break;
		case 253: // i hate it choice branch
			typewrite(speechBubblex, speechBubble1, "NO!!!", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act++; }
			break;
		case 254:
			typewrite(speechBubblex, speechBubble1, "your honesty", speech, 2);
			typewrite(speechBubblex, speechBubble2, "it shows how much", speech, 2);
			typewrite(speechBubblex, speechBubble3, "you really care", speech, 2);
			speech = false;
			if (select) { speech = true; select = 0; act = 55; }
			break;
		case 55:
			break; // music stop tension away DATE POWER
		default:
			break;
		}

		/*----------------------------------------*/

		SDL_RenderPresent(renderer);

		startTime = endTime;
		endTime = SDL_GetTicks();
	}

	SDL_DestroySemaphore(dataLock);
	dataLock = NULL;

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	IMG_Quit();
	SDL_Quit();

	return 0;
}

/*------------------------- YAY MULTITHREADING!!! --------------------------------*/

int threadFunction(void* data)
{
	Sprite egg; egg.loadFromFile("Sprites/egg.png");
	Sprite graphFrame; graphFrame.loadFromFile("Sprites/graphFrame.png");
	Sprite graph; graph.loadFromFile("Sprites/graph.png");

	SDL_Rect graphRect = { 0, 0, 120, 120 };

	SDL_Event event;

	std::cout << "second thread started";

	/*---------------------------------------------------*/

	while (!quit)
	{
		//SDL_RenderSetViewport(renderer, &viewport);
		//SDL_RenderClear(renderer);
		//typewrite(60, 440, "egg", false);
		//egg.render(50, 450, NULL);
		//SDL_RenderFillRect(renderer, &viewport);
		//graph.render(70, 50, &graphRect);
		//graphFrame.render(70, 50, NULL);
		//if (graphRect.x >= 360) { graphRect.x = 0; }
		//graphRect.x++;
		//SDL_Delay(100);
		//SDL_RenderPresent(renderer);
	}

	/*---------------------------------------------------*/

	//SDL_SemPost(dataLock); // unlock

	std::cout << "second thread ended";

	return 0;
}

void typewrite(int x_center, int y, std::string text, bool type_writer_effect, float size)
{
	Sprite words; words.loadFromFile("Sprites/text.png");
	Sprite graph; graph.loadFromFile("Sprites/graph.png");
	Sprite graphFrame; graphFrame.loadFromFile("Sprites/graphFrame.png");

	Sound textSound; textSound.loadFromFile("Audio/textSound.wav");

	SDL_Rect graphBack = { 70, 50, 120, 120 };
	SDL_Rect character[63];

	Uint32 startTime = 0;
	Uint32 endTime = 0;
	Uint32 delta = 0;
	short fps = 60;
	short timePerFrame = 1000 / 60; // miliseconds

	for (int i = 0; i < 63; i++)
	{
		character[i].x = ((i % 9) * 11);
		character[i].y = ((i / 9) * 14);
		character[i].w = 10;
		character[i].h = 13;
	}

	if (type_writer_effect) { textSound.play(text.size() - 1); }

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

		if (type_writer_effect && !datingHUD_cont)
		{
			SDL_RenderPresent(renderer);
			SDL_Delay(50);
		}
		else if (type_writer_effect && datingHUD_cont)
		{
			for (int i = 0; i < 6; i++)
			{
				if (!startTime)
					startTime = SDL_GetTicks();
				else
					delta = endTime - startTime;

				if (delta < timePerFrame)
					SDL_Delay(timePerFrame - delta);
				if (delta > timePerFrame)
					fps = 1000 / delta;

				SDL_RenderFillRect(renderer, &graphBack);
				graph.render(70, 50, &graphRect);
				graphFrame.render(70, 50, NULL);
				if (graphRect.x >= 360) { graphRect.x = 0; }
				if (i % 2) { graphRect.x++; }
				SDL_RenderPresent(renderer);

				startTime = endTime;
				endTime = SDL_GetTicks();
			}
		}
	}
}