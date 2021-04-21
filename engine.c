#include "SDLwrite.h"
#include "SDL2/SDL.h"
#include "links.h"
#include "engine.h"
#include "highscore.h"
#include <stdbool.h>
#include <stdio.h>

//gcc -o snake engine.c links.c SDLwrite.c highscore.c -lSDL2

#define INIT_SNAKE_LENGTH 10

typedef struct{
    struct link *head; //head only passed if specified
    struct link *tail; //tail passed as tail AND snake
    struct link *apple;
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDLwrite_info *info;
    int score;
    }GameState;

void draw_score(GameState *gameState);
void load_game(GameState *gameState);
bool process_events(GameState *gameState);
void render_game(GameState *gameState);
Score get_initials(GameState *gameState);
void display_scores(GameState *gameState);


int main(int argc, char *argv[])
{
    GameState gameState;
    load_game(&gameState);
    SDLwrite_info info = {PIXL_SIZE/3, PIXL_SIZE/3, 20, 240, 20, 180 };
    gameState.info = &info;

    bool done = false;

    while (!done){
        done = process_events(&gameState);
        render_game(&gameState);
        //if collided, check, get and show scores
        if (check_for_collision(gameState.tail, gameState.head)){
            if (compare_score(gameState.score)){
                Score score = get_initials(&gameState);
                save_score(score);
            }
            display_scores(&gameState);
            done = true;
        }
        //delay fps
        SDL_Delay(55);
    }
    SDL_Quit();
    return 0;
}

void load_game(GameState *gameState)
{
    gameState->window = SDL_CreateWindow("Snake",
                                         SDL_WINDOWPOS_UNDEFINED,
                                         SDL_WINDOWPOS_UNDEFINED,
                                         SCREEN_WIDTH,
                                         SCREEN_HEIGHT,
                                         0);

    gameState->renderer = SDL_CreateRenderer(gameState->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (gameState->window == NULL || gameState->renderer == NULL){
        fprintf(stderr, "Failure to create window and/or renderer\n");
        exit(EXIT_FAILURE);
    }
    gameState->score = 0;
    gameState->head = make_single_link();
    gameState->tail = make_single_link();
    
    //initial snake head coordinates
    int x = SCREEN_WIDTH / 2, y = SCREEN_HEIGHT / 2;
    
    gameState->head->x = x;
    gameState->head->y = y;
    gameState->head->next = NULL;

    gameState->tail->next = gameState->head;
    gameState->tail->x = x;
    gameState->tail->y = y += PIXL_SIZE;
    
    //initialize snake
    for (int i = 0; i < INIT_SNAKE_LENGTH - 2; i++){
        gameState->tail = add_to_snake(gameState->tail, y);
        y += PIXL_SIZE;
    }
    gameState->apple = make_apple(gameState->tail);

}

bool process_events(GameState *gameState)
{
    bool done = false;
    SDL_Event event;

    //check for events (currently exit-events)
    while (SDL_PollEvent(&event)){
        switch (event.type){
            case SDL_KEYDOWN :
                if (event.key.keysym.sym != SDLK_ESCAPE)
                    break;
            case SDL_WINDOWEVENT_CLOSE :
            case SDL_QUIT :
                done = true;
                break;
        }
    }
    if (done){
        return done;
    }
    
    //move snake's body
	struct link *s;
	struct link *n;
	for(s = gameState->tail, n = gameState->tail->next; 
		n != NULL; s = s->next, n = n->next){
	    	s->x = n->x;
			s->y = n->y;
		}
    
    //static direction - doesn't change without input
    static short int direction = 1;
    //keyboard input/move snake's head
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_LEFT] && direction != 3)
		direction = 4;
	else if (state[SDL_SCANCODE_RIGHT] && direction != 4)
		direction = 3;
	else if (state[SDL_SCANCODE_UP] && direction != 2)
		direction = 1;
	else if (state[SDL_SCANCODE_DOWN] && direction != 1)
		direction = 2;

    //move whether input was read or not
	switch(direction){
		case 4 : gameState->head->x -= PIXL_SIZE; break;
		case 3 : gameState->head->x += PIXL_SIZE; break;
		case 1 : gameState->head->y -= PIXL_SIZE; break;
		case 2 : gameState->head->y += PIXL_SIZE; break;
		default : break;
	}
    //make snake appear on opposite wall
    if (gameState->head->x > SCREEN_WIDTH - PIXL_SIZE)
		gameState->head->x = 0;
	else if (gameState->head->x < 0)
		gameState->head->x = SCREEN_WIDTH - PIXL_SIZE;
	else if (gameState->head->y > SCREEN_HEIGHT - PIXL_SIZE)
		gameState->head->y = 0;
	else if (gameState->head->y < 0)
		gameState->head->y = SCREEN_HEIGHT - PIXL_SIZE;

    //check if apple is eaten and act accordingly
    if(gameState->head->x == gameState->apple->x && 
        gameState->head->y == gameState->apple->y){
		gameState->tail = add_to_snake(gameState->tail, -1);
		free(gameState->apple);
		gameState->apple = make_apple(gameState->tail);
		gameState->score++;
	}
    return done;
}

void render_game(GameState *gameState)
{
    //set color: R, G, B, Alpha
	SDL_SetRenderDrawColor(gameState->renderer, 35, 35, 35, 255);
	//clear the screen to chosen color
	SDL_RenderClear(gameState->renderer);
	//set snake/drawing color
	SDL_SetRenderDrawColor(gameState->renderer, 255, 0, 0, 255);
	//render snake
	struct link *s;
	for(s = gameState->tail; s != NULL; s = s->next){
		const SDL_Rect rect = { s->x, s->y, PIXL_SIZE, PIXL_SIZE};
		SDL_RenderFillRect(gameState->renderer, &rect);
	}
    //render+draw score
    draw_score(gameState);

    //render+draw apple
	SDL_SetRenderDrawColor(gameState->renderer, 255, 255, 255, 255);
	SDL_Rect apple = { gameState->apple->x, gameState->apple->y, PIXL_SIZE, PIXL_SIZE};
	SDL_RenderFillRect(gameState->renderer, &apple);


	//done drawing, "present"/show on screen
	SDL_RenderPresent(gameState->renderer);
}

void draw_score(GameState *gameState)
{
    int one = gameState->score % 10;
    int two = (gameState->score % 100 - one) / 10;
    int three = (gameState->score - (one + two)) / 100;
    SDLwrite_renderDigit(gameState->renderer, gameState->info, three, 20, 20, 0, 0);
    SDLwrite_renderDigit(gameState->renderer, gameState->info, two, 20, 20, 0, 1);
    SDLwrite_renderDigit(gameState->renderer, gameState->info, one, 20, 20, 0, 2);

}

Score get_initials(GameState *gameState)
{
    Score score;
    char chars[3] = {'A', 'A', 'A'};
    int position = 0;
    bool done = false;
    while (!done){
    
	    SDL_SetRenderDrawColor(gameState->renderer, 35, 35, 35, 255);
	    SDL_RenderClear(gameState->renderer);

        SDLwrite_renderLetter(gameState->renderer, gameState->info, chars[0], SCREEN_HEIGHT / 3, SCREEN_WIDTH / 2, 0, -2);
        SDLwrite_renderLetter(gameState->renderer, gameState->info, chars[1], SCREEN_HEIGHT / 3, SCREEN_WIDTH / 2, 0, -1);
        SDLwrite_renderLetter(gameState->renderer, gameState->info, chars[2], SCREEN_HEIGHT / 3, SCREEN_WIDTH / 2, 0, 0);

        SDL_SetRenderDrawColor(gameState->renderer, 20, 240, 20, 180);
        SDL_Rect posLine = {((SCREEN_WIDTH - 83)/ 2) + 21 * position, 105, PIXL_SIZE * 1.5, PIXL_SIZE / 5};
        SDL_RenderFillRect(gameState->renderer, &posLine);

        draw_score(gameState);

        SDL_RenderPresent(gameState->renderer);

        SDL_Event event;
        SDL_PollEvent(&event);

        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_DOWN]){
            chars[position] > 'A' ? chars[position]-- : 'A';
        }
        else if (state[SDL_SCANCODE_UP]){
            chars[position] < 'Z' ? chars[position]++ : 'Z';
        }
        else if (state[SDL_SCANCODE_RIGHT]){
            position < 2 ? position++ : 2;
        }
        else if (state[SDL_SCANCODE_LEFT]){
            position > 0 ? position-- : 0;
        }
        else if (state[SDL_SCANCODE_RETURN]){
            score.c1 = chars[0];
            score.c2 = chars[1];
            score.c3 = chars[2];
            score.score = gameState->score;
            done = true;
        }
    
        SDL_Delay(100);
    }

    return score;
}

void display_scores(GameState *gameState)
{
    Score array[MAX_SCORES];
    load_scores(array);
    SDLwrite_info info = {PIXL_SIZE/4, PIXL_SIZE/4, 20, 240, 20, 180 };
    SDL_Event event;
    bool done = false;

    do {
        SDL_SetRenderDrawColor(gameState->renderer, 35, 35, 35, 255);
	    SDL_RenderClear(gameState->renderer);

        SDL_SetRenderDrawColor(gameState->renderer, 20, 240, 20, 180);

        for (int i = 0; i < MAX_SCORES; i++){
            SDLwrite_renderLetter(gameState->renderer, &info, array[i].c1,
                                  10, (SCREEN_WIDTH /4) + 30, i*1.2, 0);
            SDLwrite_renderLetter(gameState->renderer, &info, array[i].c2,
                                  10, (SCREEN_WIDTH /4) + 30, i*1.2, 1);
            SDLwrite_renderLetter(gameState->renderer, &info, array[i].c3,
                                  10, (SCREEN_WIDTH /4) + 30, i*1.2, 2);

            int one = array[i].score % 10;
            int two = (array[i].score % 100 - one) / 10;
            int three = (array[i].score - (one + two)) / 100;
            SDLwrite_renderDigit(gameState->renderer, gameState->info,
                                 three, 9, (SCREEN_WIDTH /2) + 30, i*1.2, 0);
            SDLwrite_renderDigit(gameState->renderer, gameState->info,
                                 two, 9, (SCREEN_WIDTH /2) + 30, i*1.2, 1);
            SDLwrite_renderDigit(gameState->renderer, gameState->info,
                                 one, 9, (SCREEN_WIDTH /2) + 30, i*1.2, 2);

        }
        SDL_RenderPresent(gameState->renderer);

        while (SDL_PollEvent(&event)){
            switch (event.type){
                case SDL_KEYDOWN :
                    if (event.key.keysym.sym != SDLK_ESCAPE)
                        break;
                case SDL_WINDOWEVENT_CLOSE :
                case SDL_QUIT :
                    done = true;
                    break;
            }
        }
        SDL_Delay(55);

    } while (!done);
}