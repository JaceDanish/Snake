#ifndef HIGHSCORE_H
#define HIGHSCORE_H

typedef struct{
	char c1, c2, c3;
	int score;
	}Score;

#define MAX_SCORES 10

//loads scores for printing
void load_scores(Score *array);

//compares score to top 10
//returns true if score higher than 10th
bool compare_score(int score);

//saves score in it's proper place
void save_score(Score newScore);
#endif
