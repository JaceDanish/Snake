#include <stdio.h>
#include <stdbool.h>
#include "highscore.h"

#define FILE_NAME "highscores.dat"

//Score array[MAX_SCORES];
//Score array2[MAX_SCORES];

void init_array(Score *array)
{
	Score newScore = {.c1 = 'A', .c2 = 'A', .c3 = 'A', .score = 0 };
    for(int i = 0; i < MAX_SCORES; i++){
		array[i] = newScore;
	}
}

void load_scores(Score *array)
{
    FILE *fp = fopen(FILE_NAME, "rb");

	if(fp != NULL){
		fread(array, sizeof(array[0]), MAX_SCORES, fp);
		fclose(fp);
	}else
		init_array(array);
}

bool compare_score(int score)
{
    Score array[MAX_SCORES];
    load_scores(array);
    return score > array[MAX_SCORES - 1].score;
}

void save_score(Score newScore)
{
    Score array1[MAX_SCORES];
    load_scores(array1);
    Score array2[MAX_SCORES];
    bool isPlaced = false;
	for(int i = 0, k = 0; i < MAX_SCORES; i++, k++){
		array2[i] = array1[k];
		if(isPlaced == false && array2[i].score < newScore.score){
			array2[i] = newScore;
			k--;
			isPlaced = true;
		}
	}
	FILE *fp = fopen(FILE_NAME, "wb");
	fwrite(array2, sizeof(array2[0]), MAX_SCORES, fp);
	fclose(fp);
}