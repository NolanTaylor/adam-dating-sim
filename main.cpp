#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include "Sprites.h"

bool quit = false;

SDL_sem* dataLock = false; // semaphore

int main(int argc, char* args[])
{
	window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	dataLock = SDL_CreateSemaphore(1);

	Sprite Adam("Sprites/Adam.png");
	Sprite coolBook("Sprites/book.png");
	Sprite coolHat("Sprites/coolHat.png");
	Sprite coolShirt("Sprites/coolShirt.png");
	Sprite eyes("Sprites/eyes.png");
	Sprite dialogueBox("Sprites/dialogue.png");
	Sprite bubble("Sprites/SpeechBubble.png");
	Sprite heart("Sprites/Heart.png");
	Sprite egg("Sprites/egg.png");
	Sprite graphFrame("Sprites/graphFrame.png");
	Sprite graph("Sprites/graph.png");
	Sprite radarFrame("Sprites/radarFrame.png");
	Sprite radar("Sprites/radar.png");
	Sprite tensionFrame("Sprites/tensionFrame.png");
	Sprite tensionGraph("Sprites/tensionGraph.png");
	Sprite datePowerFrame("Sprites/datePowerFrame.png");
	Sprite datePowerBar("Sprites/datePowerBar.png");

	Text dialogue;
	Text choice1;
	Text choice2;

	Sound DatingStart; DatingStart.loadFromFile("Audio/DatingStart.wav");
	Sound DatingTense; DatingTense.loadFromFile("Audio/DatingTense.wav");
	Sound DatingFight; DatingFight.loadFromFile("Audio/DatingFight.wav");
	Sound Grab; Grab.loadFromFile("Audio/Grab.wav");

	SDL_Rect eyeClip[16];
	SDL_Rect graphRect = { 0, 0, 120, 120 };
	SDL_Rect tensionRect = { 0, 0, 118, 233 };
	SDL_Rect speechBubble = { 540, 70, 240, 145 };
	SDL_Rect choice1Rect = { 270, 590, 45, 698 };
	SDL_Rect choice2Rect = { 270, 650, 44, 698 };
	SDL_Rect bar = { 0, 0, 0, 45 };

	enum eyeEnum
	{
		normal, left, right, farRight,
		suprise, twinkle, happy,
	};

	enum clothing
	{
		shirt = 1, shoulder, hat, watch, hand,
	};

	for (int i = 0; i < 16; i++)
	{
		eyeClip[i].x = (i % 4) * 86;
		eyeClip[i].y = (i / 4) * 51;
		eyeClip[i].w = 85;
		eyeClip[i].h = 50;
	}

	const Uint8* state = SDL_GetKeyboardState(NULL);

	const int xpos = 200, ypos1 = 700 - 126 + 20, ypos2 = 700 - 126 + 80;

	bool button1 = true, button2 = true, speaking = false, book = false, datePower = false, datePowerSide = false,
		speech = true, select_c = false, datingHUD = false, coolDude = false, tension = false;
	int act = 0, select = 0, count = 0, radarAngle = 0, found = NULL,
		choice_x = SCREEN_WIDTH / 2, eyePos = eyeEnum::normal,
		heart_x = xpos, heart_y = ypos1,
		choice_y1 = SCREEN_HEIGHT - dialogueBox.getHeight() + 20, // fix dialogue positions
		choice_y2 = SCREEN_HEIGHT - dialogueBox.getHeight() + 80;

	Uint32 startTime = 0;
	Uint32 endTime = 0;
	Uint32 delta = 0;
	short fps = 60;
	short timePerFrame = 1000 / 60; // miliseconds

	SDL_Event event;

	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_w:
					if (act != 67) { heart_y = ypos1; }
					break;
				case SDLK_s:
					if (act != 67) { heart_y = ypos2; }
					break;
				case SDLK_c:
					select_c = true;
					break;
				case SDLK_SPACE:
					select = 3;
					if (act != 67)
					{
						switch (heart_y)
						{
						case ypos1:
							select = 1;
							break;
						case ypos2:
							select = 2;
							break;
						}
					}
					else
					{
						if (heart_x > 330 && heart_x < 560 && heart_y > 360 && heart_y < 550 && found != clothing::shirt)
						{ found = clothing::shirt; }
						else if (heart_x > 630 && heart_x < 680 && heart_y > 350 && heart_y < 390)
						{ found = clothing::watch; }
						else if ((heart_x > 300 && heart_x < 340 && heart_y > 260 && heart_y < 330) ||
								 (heart_x > 540 && heart_x < 630 && heart_y > 270 && heart_y < 350) 
								 && found != clothing::shoulder)
						{ found = clothing::shoulder; }
						else if ((heart_x > 85 && heart_x < 220 && heart_y > 230 && heart_y < 350) ||
								 (heart_x > 650 && heart_x < 785 && heart_y > 245 && heart_y < 350)
								 && found != clothing::hand)
						{ found = clothing::hand; }
						else if (heart_x > 280 && heart_x < 490 && heart_y > 40 && heart_y < 200 && found != clothing::hat)
						{ found = clothing::hat; }
						std::cout << heart_x << "," << heart_y << std::endl;
					}
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

		if (datingHUD)
		{
			egg.render(45, 435);

			graph.render(150, 30, &graphRect);
			graphFrame.render(150, 30, NULL);
			if (graphRect.x >= 360) { graphRect.x = 0; }
			graphRect.x += 1;

			radar.renderEx(710, 380, NULL, 1, radarAngle);
			radarFrame.render(710, 380);
			if (radarAngle > 360) { radarAngle = 0; }
			radarAngle += 1;
		}

		if (tension)
		{
			tensionGraph.render(70, 100, &tensionRect);
			tensionFrame.render(70, 100, NULL);
			if (tensionRect.x >= 381) { tensionRect.x = 0; }
			tensionRect.x += 2;
		}

		if (datePower)
		{
			datePowerFrame.render(330, 10, NULL);
			datePowerBar.render(333, 13, &bar);
		}
		else if (datePowerSide)
		{
			datePowerFrame.render(70, 50, NULL);
			datePowerBar.render(73, 53, &bar);
		}

		Adam.render((SCREEN_WIDTH - Adam.getWidth()) / 2, SCREEN_HEIGHT - Adam.getHeight() - dialogueBox.getHeight());

		if (book)
			coolBook.render(80, 111, NULL);
		else if (coolDude)
		{
			coolHat.render(279, 51);
			coolShirt.render(240, 246, NULL);
		}

		eyes.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + 275, 185, &eyeClip[eyePos]);
		dialogueBox.render((SCREEN_WIDTH - dialogueBox.getWidth()) / 2, SCREEN_HEIGHT - dialogueBox.getHeight() - 1, NULL);

		if (speaking)
			bubble.render(500, 50, NULL);

		if (dialogue.getTyping())
		{
			if (dialogue.type(speechBubble, speech, 2))
			{
				dialogue.setString("");
				speech = false;
			}
			else
				select = 0;
		}

		if (choice1.getTyping())
		{
			if (act >= 196 && act <= 198)
			{
				if (choice1.type(choice1Rect, speech, 2))
				{
					choice1.setString("");
					speech = false;
				}
			}
			else
			{
				choice1.type(choice1Rect, false, 2);
				choice2.type(choice2Rect, false, 2);
				choice1.setString("");
				choice2.setString("");
			}
		}

		if (act == 67)
		{
			if (state[SDL_SCANCODE_W]) { heart_y -= 1; }
			if (state[SDL_SCANCODE_A]) { heart_x -= 1; }
			if (state[SDL_SCANCODE_S]) { heart_y += 1; }
			if (state[SDL_SCANCODE_D]) { heart_x += 1; }
			heart.render(heart_x, heart_y, NULL);
		}

		switch (act)
		{
		case 0:
			choice1.setString("wasd to move");
			choice2.setString("space to select advance");
			heart.render(heart_x, heart_y, NULL);
			if (select) { act++; select = 0; speaking = true; }
			break;
		case 1:
			dialogue.setString("So umm...");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 2:
			dialogue.setString("If you've seen/everything...");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 3:
			dialogue.setString("Do you want to/start the date?");
			if (select) { dialogue.setString(""); speech = true; select = 0; act++; }
			break;
		case 4:
			choice1.setString("yes");
			choice2.setString("no");
			heart.render(heart_x, heart_y, NULL);
			if (select) { select = 0; act++; heart_y = ypos1; }
			break;
		case 5:
			speaking = true;
			act++;
			break;
		case 6:
			dialogue.setString("Okay!/Dating Start!");
			if (select) { dialogue.setString(""); speech = true; speaking = false; select = 0; act++; }
			break;
		case 7: // DATING START!!! (fix);
			choice1.setString("DATING START");
			if (select) { speaking = true; select = 0; DatingStart.play(1, 1); act++; }
			break;
		case 8:
			dialogue.setString("Here we are/on our date!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 9:
			dialogue.setString("I've actually/never done this/before");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 10:
			dialogue.setString("But don't worry!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 11:
			dialogue.setString("You can't spell/prepared without/several letters/from my name");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 12:
			Grab.play(2, 0);
			for (int i = 0; i <= 360; i++)
			{
				SDL_RenderClear(renderer);
				Adam.renderEx((SCREEN_WIDTH - Adam.getWidth()) / 2,
					SCREEN_HEIGHT - Adam.getHeight() - dialogueBox.getHeight(), NULL, 1, i, NULL);
				dialogueBox.render((SCREEN_WIDTH - dialogueBox.getWidth()) / 2,
					SCREEN_HEIGHT - dialogueBox.getHeight() - 1);
				SDL_RenderPresent(renderer); SDL_Delay(1);
			} act++; book = true;
			break;
		case 13:
			dialogue.setString("I snagged an/official dating/rulebook from/the library!!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 14:
			dialogue.setString("We're ready to/have a great/time!!!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 15:
			dialogue.setString("Let's see...");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 16:
			dialogue.setString("Step 1: Press/the 'c' key on/your keyboard/for dating hud");
			if (select_c) { speech = true; datingHUD = true; select_c = false; select = 0; act++; }
			break;
		case 17:
			for (int i = 0; i <= 190; i += 1)
			{
				SDL_RenderClear(renderer);
				Adam.render((SCREEN_WIDTH - Adam.getWidth()) / 2, SCREEN_HEIGHT - Adam.getHeight() - dialogueBox.getHeight());
				coolBook.render(80, 111);
				dialogueBox.render((SCREEN_WIDTH - dialogueBox.getWidth()) / 2, SCREEN_HEIGHT - dialogueBox.getHeight() - 1, NULL);
				if (i <= 45) { egg.render(i, 435); }
				else { egg.render(45, 435); }
				if (i <= 150) { graphFrame.render(i, 30); graph.render(i, 30, &graphRect); }
				else { graphFrame.render(150, 30); graph.render(150, 30, &graphRect); }
				if (i <= 190) { radarFrame.render(900 - i, 380); radar.render(900 - i, 380); }
				else { radarFrame.render(710, 380); radar.render(710, 380); }
				SDL_RenderPresent(renderer); SDL_Delay(6);
			} act++;
			break;
		case 18:
			dialogue.setString("Wowie!/I feel so/informed!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 19:
			dialogue.setString("I think we're/ready for/step 2!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 20:
			dialogue.setString("Step 2: Ask/them on a date");
			if (select) { book = false; speech = true; select = 0; act++; }
			break;
		case 21:
			dialogue.setString("Ahem.");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 22:
			dialogue.setString("Human,/I the great/Adam");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 23:
			dialogue.setString("Will go on a/date with you!!");
			if (select) { dialogue.setString(""); speech = true; select = 0; act++; }
			break;
		case 24:
			choice1.setString("yes");
			choice2.setString("no");
			heart.render(heart_x, heart_y, NULL);
			if (select == 1) { eyePos = eyeEnum::twinkle; act = 125; }
			else if (select == 2) { act = 225; heart_y = ypos1; }
			speech = true; select = 0; break;
		case 125: // yes choice branch
			dialogue.setString("R-Really?/Wowie!!!");
			if (select) { eyePos = eyeEnum::normal; speech = true; select = 0; act = 26; }
			break;
		case 225: // no choice branch
			dialogue.setString("Fortunately it/only says to ask");
			if (select) { speech = true; select = 0; act = 26; }
			break;
		case 26:
			dialogue.setString("I guess that/means it's time/for part three!");
			if (select) { book = true; speech = true; select = 0; act++; }
			break;
		case 27:
			dialogue.setString("Step 3: Put on/nice clothes to/show you care");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 28:
			dialogue.setString("...");
			if (select) { book = false; speech = true; select = 0; act++; }
			break;
		case 29:
			DatingStart.stop(1);
			dialogue.setString("Wait a second...");
			if (select) { speech = true; select = 0; DatingTense.play(3, 1); act++; }
			break;
		case 30:
			dialogue.setString("'Wear clothing'");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 31:
			dialogue.setString("You're wearing/clothing right/now!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 32:
			dialogue.setString("Not only that...");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 33:
			dialogue.setString("Earlier today/you were also/wearing/clothing!!!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 34:
			dialogue.setString("No!!!/Could it be!?!?");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 35:
			dialogue.setString("You've wanted/to date me from/the very/beginning?!?!");
			if (select) { dialogue.setString(""); speech = true; select = 0; act++; }
			break;
		case 36:
			choice1.setString("yes");
			choice2.setString("no");
			heart.render(heart_x, heart_y, NULL);
			if (select == 1) {
				act++; DatingTense.stop(3);
				eyePos = eyeEnum::suprise; datingHUD = false;
			} // also play shake sfx
			else if (select == 2) {
				act++; DatingTense.stop(3); heart_y = ypos1;
				eyePos = eyeEnum::suprise; datingHUD = false;
			}
			speech = true; break;
		case 37:
			std::cout << "adam is a big boi shaek";
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderClear(renderer);
			SDL_RenderPresent(renderer); SDL_Delay(200);
			SDL_SetRenderDrawColor(renderer, 20, 20, 50, 255);
			for (int i = 1; i < 40; i *= -2)
			{
				SDL_RenderClear(renderer);
				Adam.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + i, SCREEN_HEIGHT - Adam.getHeight() - dialogueBox.getHeight());
				eyes.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + 275 + i, 185, &eyeClip[eyePos]);
				SDL_RenderPresent(renderer); SDL_Delay(100);
			}
			for (int i = 40; abs(i) > 0; i *= -0.5)
			{
				SDL_RenderClear(renderer);
				Adam.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + i, SCREEN_HEIGHT - Adam.getHeight() - dialogueBox.getHeight());
				eyes.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + 275 + i, 185, &eyeClip[eyePos]);
				SDL_RenderPresent(renderer); SDL_Delay(100);
			} SDL_Delay(200);
			switch (select)
			{
			case 1:
				act = 138; break;
			case 2:
				act = 238; break;
			}
			select = 0; break;
		case 138: // yes choice branch
			dialogue.setString("NO!!!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 139:
			dialogue.setString("You planned/it all!!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 140:
			dialogue.setString("You're way better/at dating than/I am");
			if (select) { speech = true; select = 0; act = 41; }
			break;
		case 238: // no choice branch Music stop datingHUD away Shake
			eyePos = eyeEnum::normal;
			dialogue.setString("Despite that you/chose to wear/clothing today/of all days");
			if (select) { eyePos = eyeEnum::happy; speech = true; select = 0; act++; }
			break;
		case 239:
			dialogue.setString("Was your/interest in/me...");
			if (select) { eyePos = eyeEnum::twinkle; speech = true; select = 0; act++; }
			break;
		case 240:
			dialogue.setString("Predestined!?!?");
			if (select) { eyePos = eyeEnum::suprise; speech = true; select = 0; act = 41; }
			break;
		case 41:
			dialogue.setString("N-NOOOO!!!/Your/DATING POWER!!!");
			if (select) { eyePos = eyeEnum::normal; speech = true; select = 0; act++; }
			break;
		case 42:
			std::cout << "adam is a big boi shaek";
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderClear(renderer);
			SDL_RenderPresent(renderer); SDL_Delay(200);
			SDL_SetRenderDrawColor(renderer, 20, 20, 50, 255);
			for (int i = 1; i < 40; i *= -2)
			{
				SDL_RenderClear(renderer);
				Adam.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + i, SCREEN_HEIGHT - Adam.getHeight() - dialogueBox.getHeight());
				eyes.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + 275 + i, 185, &eyeClip[eyePos]);
				datePowerFrame.render(330, 10, NULL);
				datePowerBar.render(333, 13, &bar);
				bar.w += 4;
				SDL_RenderPresent(renderer); SDL_Delay(100);
			}
			for (int i = 40; abs(i) > 0; i *= -0.5)
			{
				SDL_RenderClear(renderer);
				Adam.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + i, SCREEN_HEIGHT - Adam.getHeight() - dialogueBox.getHeight());
				eyes.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + 275 + i, 185, &eyeClip[eyePos]);
				datePowerFrame.render(330, 10, NULL);
				datePowerBar.render(333, 13, &bar);
				bar.w += 4;
				SDL_RenderPresent(renderer); SDL_Delay(100);
			}
			Adam.render((SCREEN_WIDTH - Adam.getWidth()) / 2, SCREEN_HEIGHT - Adam.getHeight() - dialogueBox.getHeight());
			eyes.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + 275, 185, &eyeClip[eyePos]);
			SDL_RenderPresent(renderer);
			for (int i = 0; i < 10; i++)
			{
				SDL_RenderClear(renderer);
				Adam.render((SCREEN_WIDTH - Adam.getWidth()) / 2, SCREEN_HEIGHT - Adam.getHeight() - dialogueBox.getHeight());
				eyes.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + 275, 185, &eyeClip[eyePos]);
				datePowerFrame.render(330, 10, NULL);
				datePowerBar.render(333, 13, &bar);
				bar.w += 4;
				SDL_RenderPresent(renderer); SDL_Delay(100);
			}
			SDL_RenderClear(renderer);
			SDL_Delay(500); act++; break;
		case 43:
			dialogue.setString("Nyeh");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 44:
			dialogue.setString("Nyeh heh heh");
			if (select) { speech = true; select = 0; DatingFight.play(4, 1); tension = true; act++; }
			break;
		case 45: // DatingFight tension graph and music
			dialogue.setString("Don't think/you've bested/me yet!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 46:
			dialogue.setString("I the great/Adam!!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 47:
			dialogue.setString("Have never been/beaten at/dating and I/never will!!!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 48:
			dialogue.setString("I can easily/keep up with/you!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 49:
			dialogue.setString("You see;/I too can wear/clothing!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 50:
			dialogue.setString("In fact!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 51:
			dialogue.setString("I always wear my/SPECIAL CLOTHES/underneath my/regular clothes");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 52:
			dialogue.setString("Just in case/somebody happens/to ask me on a/date!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 53:
			dialogue.setString("Behold!!!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 54:
			for (int i = 0; i < 900; i+=2)
			{
				SDL_RenderClear(renderer);
				Adam.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + i, SCREEN_HEIGHT - Adam.getHeight() - dialogueBox.getHeight());
				eyes.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + 275 + i, 185, &eyeClip[eyePos]);
				SDL_RenderPresent(renderer);
				SDL_Delay(1);
			}
			for (int i = 0; i < 900; i+=2)
			{
				SDL_RenderClear(renderer);
				Adam.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + 900 - i, SCREEN_HEIGHT - Adam.getHeight() - dialogueBox.getHeight());
				coolHat.render(279 + 900 - i, 51);
				coolShirt.render(240 + 900 - i, 246, NULL);
				eyes.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + 1175 - i, 185, &eyeClip[eyePos]);
				SDL_RenderPresent(renderer);
				SDL_Delay(1);
			}
			coolDude = true; act++;
			break; // ADAM MOVE OUT!!!
		case 55:
			dialogue.setString("Nyeh!/What do you/think of my/secret style");
			if (select) { dialogue.setString(""); speaking = false; speech = true; select = 0; act++; }
			break;
		case 56:
			choice1.setString("I love it");
			choice2.setString("I hate it");
			heart.render(heart_x, heart_y, NULL);
			if (select == 1) { act = 157; eyePos = eyeEnum::suprise; }
			else if (select == 2) { act = 257; heart_y = ypos1; eyePos = eyeEnum::suprise; }
			speech = true; select = 0; speaking = true;
			break;
		case 157: // i love it choice branch
			dialogue.setString("NO!!!!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 158:
			dialogue.setString("A genuine/compliment!!");
			if (select) { speech = true; select = 0; act = 59; }
			break;
		case 257: // i hate it choice branch
			dialogue.setString("NO!!!!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 258:
			dialogue.setString("Your honesty!!/It shows how/much you really/care!!!");
			if (select) { speech = true; select = 0; act = 59; }
			break;
		case 59:
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderClear(renderer);
			SDL_RenderPresent(renderer); SDL_Delay(200);
			SDL_SetRenderDrawColor(renderer, 20, 20, 50, 255);
			for (int i = 1; i < 40; i *= -2)
			{
				SDL_RenderClear(renderer);
				Adam.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + i, SCREEN_HEIGHT - Adam.getHeight() - dialogueBox.getHeight());
				coolHat.render(279 + i, 51, NULL);
				coolShirt.render(240 + i, 246, NULL);
				eyes.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + 275 + i, 185, &eyeClip[eyePos]);
				datePowerFrame.render(330, 10, NULL);
				datePowerBar.render(333, 13, &bar);
				bar.w += 3;
				SDL_RenderPresent(renderer); SDL_Delay(100);
			}
			for (int i = 40; abs(i) > 0; i *= -0.5)
			{
				SDL_RenderClear(renderer);
				Adam.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + i, SCREEN_HEIGHT - Adam.getHeight() - dialogueBox.getHeight());
				coolHat.render(279 + i, 51, NULL);
				coolShirt.render(240 + i, 246, NULL);
				eyes.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + 275 + i, 185, &eyeClip[eyePos]);
				datePowerFrame.render(330, 10, NULL);
				datePowerBar.render(333, 13, &bar);
				bar.w += 5;
				SDL_RenderPresent(renderer); SDL_Delay(100);
			}
			Adam.render((SCREEN_WIDTH - Adam.getWidth()) / 2, SCREEN_HEIGHT - Adam.getHeight() - dialogueBox.getHeight());
			coolHat.render(279, 51, NULL);
			coolShirt.render(240, 246, NULL);
			eyes.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + 275, 185, &eyeClip[eyePos]);
			SDL_RenderPresent(renderer);
			for (int i = 0; i < 10; i++)
			{
				SDL_RenderClear(renderer);
				Adam.render((SCREEN_WIDTH - Adam.getWidth()) / 2, SCREEN_HEIGHT - Adam.getHeight() - dialogueBox.getHeight());
				coolHat.render(279, 51, NULL);
				coolShirt.render(240, 246, NULL);
				eyes.render((SCREEN_WIDTH - Adam.getWidth()) / 2 + 275, 185, &eyeClip[eyePos]);
				datePowerFrame.render(330, 10, NULL);
				datePowerBar.render(333, 13, &bar);
				bar.w += 5;
				SDL_RenderPresent(renderer); SDL_Delay(100);
			}
			SDL_RenderClear(renderer);
			act++; tension = false; datePower = true; eyePos = eyeEnum::normal;
			break; // music stop tension away DATE POWER
		case 60:
			dialogue.setString("However...");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 61:
			dialogue.setString("You don't truly/understand the/HIDDEN POWER/of this outfit!!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 62:
			dialogue.setString("Therefore...");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 63:
			dialogue.setString("What you just/said is/invalid!!!");
			if (bar.w > 110)
			{
				bar.w--;
			}
			else
				if (select) { speech = true; select = 0; act++; }
			break;
		case 64:
			dialogue.setString("This date won't/escalate any/further!!!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 65:
			dialogue.setString("... Unless/you find my/secret!!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 66:
			dialogue.setString("But that won't/happen!");
			if (select) { dialogue.setString(""); datePower = false; speech = true; speaking = false; select = 0; act++; }
			break;
		case 67:
			choice1.setString("Move and inspect with space");
			switch (found)
			{
			case clothing::shirt:
				speaking = true; speech = true; select = 0; act = 168;
				break;
			case clothing::shoulder:
				speaking = true; speech = true; select = 0; act = 268;
				break;
			case clothing::hat:
				speaking = true; speech = true; select = 0; act = 368;
				break;
			case clothing::watch:
				speaking = true; speech = true; select = 0; act = 468;
				break;
			case clothing::hand:
				speaking = true; speech = true; select = 0; act = 568;
				break;
			}
			break;
		case 168:
			dialogue.setString("This shirt/didn't/originally say/'cool' but I/improved it");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 169:
			dialogue.setString("Expert tip: All/articles of/clothing can be/improved this/way!!");
			if (select) { dialogue.setString(""); found = NULL; speech = true; speaking = false; select = 0; act = 67; }
			break;
		case 268:
			dialogue.setString("I see. I see.");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 269:
			dialogue.setString("You like/caressing my/biceps with a/floating heart");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 270:
			dialogue.setString("But who/doesn't!?");
			if (select) { dialogue.setString(""); found = NULL; speech = true; speaking = false; select = 0; act = 67; }
			break;
		case 368:
			dialogue.setString("My hat...?");
			if (select) { speech = true; select = 0; act = 69; }
			break;
		case 468:
			dialogue.setString("They say watches/hold the secret/to time and/space...");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 469:
			dialogue.setString("But mine just/has pictures/of cats...");
			if (select) { dialogue.setString(""); found = NULL; speech = true; speaking = false; select = 0; act = 67; }
			break;
		case 568:
			dialogue.setString("Holding my hand/so I'll tell you/the answer...");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 569:
			dialogue.setString("No!!/I must resist!");
			if (select) { dialogue.setString(""); found = NULL; speech = true; speaking = false; select = 0; act = 67; }
			break;
		case 69:
			dialogue.setString("My hat.");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 70:
			dialogue.setString("My hat!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 71:
			dialogue.setString("Nyeh heh heh!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 72:
			//lift hat; reveal present.
			act++;
			break;
		case 73:
			dialogue.setString("W-Well then.../You found my/secret!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 74:
			dialogue.setString("I suppose I have/no choice!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 75:
			dialogue.setString("It's a/present...");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 76:
			dialogue.setString("A present/j-just for you!");
			if (select) { dialogue.setString(""); heart_x = xpos; heart_y = ypos1; speech = true; select = 0; act++; }
			break;
		case 77:
			choice1.setString("Open it");
			choice2.setString("Don't");
			heart.render(heart_x, heart_y, NULL);
			if (select == 1) { select = 0; act = 82; }
			else if (select == 2) { select = 0; heart_y = ypos1; act = 278; }
			speech = true; break;
		case 278: //Don't choice branch
			dialogue.setString("You can't even/bring yourself/to harm my/delicate/wrapping??");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 279:
			dialogue.setString("N-NO!/That technique!!");
			if (select) { speech = true; eyePos = eyeEnum::suprise; select = 0; act++; }
			break;
		case 280:
			dialogue.setString("It's too strong!");
			if (select) { speech = true; eyePos = eyeEnum::normal; select = 0; act++; } //scowl eyePos?!??!
			break;
		case 281:
			dialogue.setString("Counterattack!/I'll open the/present myself!!");
			if (select) { speech = true; select = 0; act = 82; }
			break;
		case 82:
			dialogue.setString("Do you know/what this is?");
			if (select) { dialogue.setString(""); speech = true; select = 0; act++; }
			break;
		case 83:
			choice1.setString("Of course");
			choice2.setString("No idea");
			heart.render(heart_x, heart_y, NULL);
			if (select == 1) { select = 0; act = 184; }
			else if (select == 2) { select = 0; heart_y = ypos1; act = 284; }
			speech = true; break;
		case 184: // Of course choice branch
			dialogue.setString("'Spaghetti'");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 185:
			dialogue.setString("That's what/you're thinking/isn't it?");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 186:
			dialogue.setString("Right!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 187:
			dialogue.setString("But oh so/wrong!!!");
			if (select) { speech = true; select = 0; act = 88; }
			break;
		case 284:
			dialogue.setString("Nyeh heh heh!/That's right!!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 285:
			dialogue.setString("You have no/idea!!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 286:
			dialogue.setString("Though this/appears to be/spaghetti...");
			if (select) { speech = true; select = 0; act = 88; }
			break;
		case 88:
			for (int i = 0; i < 100; i += 5)
			{
				SDL_RenderClear(renderer);
				Adam.render((SCREEN_WIDTH - Adam.getWidth()) / 2, SCREEN_HEIGHT - Adam.getHeight() - dialogueBox.getHeight());
				coolHat.render(279, 51, NULL);
				coolShirt.render(240, 246, NULL);
				datePowerFrame.render(i, 50);
				datePowerBar.render(i + 3, 53, &bar);
			}
			datePowerSide = true;
			act++; break;
		case 89:
			dialogue.setString("This ain't any/plain ol' pasta!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 90:
			dialogue.setString("This is an/artisan's work!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 91:
			dialogue.setString("Silken/spaghetti,/finely aged in/an oaken cask...");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 92:
			dialogue.setString("Then cooked by/me, master/chef Adam!!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 93:
			dialogue.setString("Human!!!/It's time to/end this!!");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 94:
			dialogue.setString("There's no way/this can go/any further!");
			if (select) { dialogue.setString(""); speech = true; select = 0; act++; }
			break;
		case 95:
			choice1.setString("Eat it");
			choice2.setString("Refuse");
			heart.render(heart_x, heart_y, NULL);
			if (select == 1) { select = 0; act = 196; }
			else if (select == 2) { select = 0; heart_y = ypos1; act = 296; }
			speech = true; break;
		case 196:
			choice1.setString("*You take a small bite");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 197:
			choice1.setString("*Your face reflexively scrunches up");
			if (select) { speech = true; select = 0; act++; }
			break;
		case 198:
			choice1.setString("*The taste is indescribable...");
			if (select) { eyePos = eyeEnum::happy; speech = true; select = 0; act++; }
			break;
		case 199:
			dialogue.setString("What a passionate/expression!!!");
			if (select) { eyePos = eyeEnum::twinkle; speech = true; select = 0; act = 1100; }
			break;
		case 1100:
			dialogue.setString("You must really/love my cooking!");
			if (select) { eyePos = eyeEnum::happy; speech = true; select = 0; act++; }
			break;
		case 1101:
			dialogue.setString("Any by extension/me!!!");
			if (select) { eyePos = eyeEnum::suprise; speech = true; select = 0; act++; }
			break;
		case 1102:
			dialogue.setString("Maybe even more/than I do!!");
			if (select) { speech = true; select = 0; act++; } // set act!
			break;
		case 296:
			dialogue.setString("You mean...");
			if (select) { speech = true; select = 0; act++; }
			break;
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