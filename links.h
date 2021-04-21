#ifndef LINKS_H
#define LINKS_H
#include <stdbool.h>

//Simple linked list-node with pixel coordinates
struct link{
	int x, y;
	struct link *next;
	};

//Adds nodes to snake; y = -1 unless game is being initialized
struct link *add_to_snake(struct link *snake, int y);

//Initialize snake-head
struct link *make_single_link();

//Returns struct with random coordinates outside of snake
struct link *make_apple(struct link *snake);

//Checks for collision between head and tail
bool check_for_collision(struct link *snake, struct link *head);

//Frees entire snake. Optional if game ends anyway
void free_links(struct link *tail);

#endif