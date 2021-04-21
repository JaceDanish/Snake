#include "links.h"
#include "engine.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

struct link *add_to_snake(struct link *snake, int y)
{
    struct link *link = make_single_link();
    //if adding to existing snake
    if (y < 0){
        link->x = -100; //out of sight, out of mind
        link->y = -100;
    //else initializing new snake
    }else{
        link->x = snake->x;
        link->y = y;
    }
    link->next = snake;
    return link;
}

struct link *make_single_link()
{
    struct link *link = NULL;
    link = malloc(sizeof(struct link));
    if (link == NULL){
        fprintf(stderr, "Error: memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return link;
}

struct link *make_apple(struct link *snake)
{
    //make apple and randomize location
    srandom((int)time(NULL));
    struct link *s;
    struct link *apple = make_single_link();
    apple->x = random()%(SCREEN_WIDTH / PIXL_SIZE) * PIXL_SIZE;
    apple->y = random()%(SCREEN_HEIGHT / PIXL_SIZE) * PIXL_SIZE;
    //check if location is clear of snake; re-randomize if not
    checkAgain: //if apple is repositioned
    for (s = snake; s != NULL; s = s->next){
        if (s->x == apple->x && s->y == apple->y){
            apple->x = random()%(SCREEN_WIDTH / PIXL_SIZE) * PIXL_SIZE;
            apple->y = random()%(SCREEN_HEIGHT / PIXL_SIZE) * PIXL_SIZE;
            goto checkAgain; //starts over from for-loop
        }
    }
    return apple;
}

bool check_for_collision(struct link *snake, struct link *head)
{
    bool collision = false;
    struct link *s;
    struct link *n;
    for (s = snake, n = snake->next; n != NULL; s = s->next, n = n->next){
        if (s->x == head->x && s->y == head->y){
            collision = true;
        }
    }
    return collision;
}

void free_links(struct link *tail)
{
    struct link *s;
    struct link *n;
    for (s = tail, n = tail->next; n != NULL; s = n, n = n->next){
        free(s);
    }
}