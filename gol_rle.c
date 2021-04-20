#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "rle.h"

#define SEED    3141592

#define RUN_UNTIL_GENERATION 1000

#define MILI_TO_MICRO(x) (x * 1000)

#define ALIVE   '0'
#define DEAD    '.'

#define ROWS    80
#define COLUMNS 150

void generation(void);
int get_alive_neighbours(int cell_position_row, int cell_position_column);
void random_grid(void);
void draw_frame(void);
void print_grid(char *p_grid, const int rows, const int columns);
void fill_grid_rle(rle_t *rle, char *p_grid, const int rows, const int columns);

int generation_count = 0;

char grid[ROWS][COLUMNS];
char grid_temp[ROWS][COLUMNS];

rle_t rle;

void main(int argc, char **argv)
{ 
    if(argc < 2)
    {
        return;
    }

    rle_init(&rle);
    rle_get(&rle, argv[1]);

    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLUMNS; j++)
        {
            grid[i][j] = DEAD;
        }   
    }

    fill_grid_rle(&rle, (char*)&grid[0][0], ROWS, COLUMNS);
    //print_grid((char*)grid, ROWS, COLUMNS);

    //draw_frame();

    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLUMNS; j++)
        {
            grid_temp[i][j] = grid[i][j];
        }   
    }

    while(generation_count <= RUN_UNTIL_GENERATION)
    {
        system("clear");
        printf("GENERATION: %d \t PATTERN NAME: %s", generation_count, rle.pattern_name);
        print_grid((char*)grid, ROWS, COLUMNS);
        generation();
        usleep(MILI_TO_MICRO(100));
        //getchar();
    }

    rle_deinit(&rle);
}

void generation(void)
{
    generation_count++;

    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLUMNS; j++)
        {
            int neighbors = get_alive_neighbours(i, j);

            if(grid[i][j] == ALIVE && (neighbors == 2 || neighbors == 3))
            {
                grid_temp[i][j] = ALIVE;
            }
            else if(grid[i][j] == DEAD && neighbors == 3)
            {
                grid_temp[i][j] = ALIVE;
            }
            else
            {
                grid_temp[i][j] = DEAD;
            }
        }
    }

    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLUMNS; j++)
        {
            grid[i][j] = grid_temp[i][j];
        }   
    }
}

int get_alive_neighbours(int cell_position_row, int cell_position_column)
{
    int count = 0;

    //printf("\n\n Origin %d|%d", cell_position_row, cell_position_column);

    for(int i = (cell_position_row-1); i <= (cell_position_row+1); i++)
    {
        for(int j = (cell_position_column-1); j <= (cell_position_column+1); j++)
        {
            //printf("\n Checking %d|%d", i, j);
            
            if(grid[i][j] == ALIVE && (i != cell_position_row || j != cell_position_column))
            {
                count++;

                //printf("\n Neighbor on: %d|%d", i, j);
                //getchar();
            }
        }   
    }

    return count;
}

void random_grid(void)
{
    srandom(SEED);

    for(int i = 1; i < ROWS-1; i++)
    {
        for(int j = 1; j < COLUMNS-1; j++)
        {
            //grid[i][j] = (random() % 3 == 0) ? ALIVE : DEAD;
            grid[i][j] = DEAD;
        }
    }
    
    // grid[34][68] = ALIVE;
    // grid[35][66] = ALIVE;
    // grid[35][68] = ALIVE;
    // grid[35][69] = ALIVE;
    // grid[36][66] = ALIVE;
    // grid[36][68] = ALIVE;
    // grid[37][66] = ALIVE;
    // grid[38][64] = ALIVE;
    // grid[39][62] = ALIVE;
    // grid[39][64] = ALIVE;

    // grid[34][68] = ALIVE;
    // grid[34][69] = ALIVE;
    // grid[34][70] = ALIVE;
    // grid[34][72] = ALIVE;
    // grid[35][68] = ALIVE;
    // grid[36][71] = ALIVE;
    // grid[36][72] = ALIVE;
    // grid[37][69] = ALIVE;
    // grid[37][70] = ALIVE;
    // grid[37][72] = ALIVE;
    // grid[38][68] = ALIVE;
    // grid[38][70] = ALIVE;
    // grid[38][72] = ALIVE;

    grid[34][70] = ALIVE;
    grid[34][75] = ALIVE;
    grid[35][68] = ALIVE;
    grid[35][69] = ALIVE;
    grid[35][71] = ALIVE;
    grid[35][72] = ALIVE;
    grid[35][73] = ALIVE;
    grid[35][74] = ALIVE;
    grid[35][76] = ALIVE;
    grid[35][77] = ALIVE;
    grid[36][70] = ALIVE;
    grid[36][75] = ALIVE;
}

void draw_frame(void)
{
    grid[0][0] = '+';
    grid[0][COLUMNS-1] = '+';
    grid[ROWS-1][0] = '+';
    grid[ROWS-1][COLUMNS-1] = '+';

    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLUMNS; j++)
        {
            if((i == 0 || i == (ROWS-1)) && (j > 0 && j < (COLUMNS-1)))
            {
                grid[i][j] = '-';
            }
            else if((i > 0 && i < (ROWS-1)) && (j == 0 || j == (COLUMNS-1)))
            {
                grid[i][j] = '|';
            }
        }   
    }
}

void print_grid(char *p_grid, const int rows, const int columns)
{
    printf("\n");
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < columns; j++)
        {
            printf("%c", *((p_grid+i*columns) + j));
        }
        printf("\n");
    }
}

void fill_grid_rle(rle_t *rle, char *p_grid, const int rows, const int columns)
{
    char *ptr_temp = NULL;
    char *line_start = NULL;

    if(columns < rle->size_x || rows < rle->size_y)
    {
        return;       
    }

    ptr_temp = p_grid;
    line_start = ptr_temp;

    for(int i = 0; i < rle->tokens_count; i++)
    {
        //printf("\nPlacing '%c' repeating %d times starting on %d", rle->tokens[i].token, rle->tokens[i].repetition, (ptr_temp - p_grid));

        if(rle->tokens[i].token == 'b')
        {
            memset(ptr_temp, DEAD, rle->tokens[i].repetition);
            ptr_temp += rle->tokens[i].repetition;
        }
        else if(rle->tokens[i].token == 'o')
        {
            memset(ptr_temp, ALIVE, rle->tokens[i].repetition);
            ptr_temp += rle->tokens[i].repetition;
        }
        else if((rle->tokens[i].token == '$'))
        {
            ptr_temp += (columns - (ptr_temp - line_start));
            line_start = ptr_temp;
        }

        //getchar();
    }
}