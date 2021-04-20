#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rle.h"

#define FILE_CONTENT_DEBUG

static char line_read[256];

static void remove_crlf(char *str)
{
    char *ptr_temp = NULL;

    if(str == NULL)
        return;

    ptr_temp = str;

    while(*ptr_temp != '\0')
    {
        if(*ptr_temp == 0x0D || *ptr_temp == 0x0A) *ptr_temp = '\0';
        
        *ptr_temp++;
    }
}

int rle_get(rle_t *rle, char *filename)
{
    FILE *file;

    if(rle == NULL || filename == NULL)
    {
        return 1;
    }

    if(rle->pattern_name == NULL || rle->tokens == NULL)
        return 1;

    file = fopen(filename, "r");

    if(file == NULL)
    {
        return 1;
    }
    else
    {
        while(fgets(line_read, sizeof(line_read), file) != NULL)
        {
            remove_crlf(line_read);
            rle_message_parser(rle, line_read);
        }
    }

#ifdef FILE_CONTENT_DEBUG
    printf("\nPattern name: %s", rle->pattern_name);
    printf("\nSize X: %d", rle->size_x);
    printf("\nSize Y: %d", rle->size_y);
    printf("\nRule: %d", rle->rule);
    printf("\nTokens count: %d", rle->tokens_count);

    for(int i = 0; i < rle->tokens_count; i++)
    {
        printf("\nToken: %c Repetitions: %d", rle->tokens[i].token, rle->tokens[i].repetition);
    }

    printf("\n");
#endif    

    fclose(file);
}

int rle_init(rle_t *rle)
{
    if(rle == NULL)
        return 1;

    rle->pattern_name   = (char*)malloc(1);
    rle->tokens         = (rle_token_t*)malloc(sizeof(rle_token_t));

    if(rle->pattern_name == NULL || rle->tokens == NULL)
        return 0;

    return 0;
}

int rle_deinit(rle_t *rle)
{
    if(rle == NULL)
        return 1;

    if(rle->pattern_name != NULL)   free(rle->pattern_name);
    if(rle->tokens != NULL)         free(rle->tokens);

    return 0;
}

int rle_message_parser(rle_t *rle, char *line_read)
{
    char first_character = line_read[0];

    if(rle == NULL)
        return 1;

    if(first_character == '#')
    {
        rle_hash_parser(rle, line_read);
    }
    else if(strstr(line_read, "x") && strstr(line_read, "y") && strstr(line_read, "rule"))
    {
        rle_size_rule_parser(rle, line_read);
    }
    else
    {
        rle_tokens_parser(rle, line_read);
    }

    return 0;
}

int rle_hash_parser(rle_t *rle, char *line_read)
{
    char *ptr_temp = NULL;
    char token = 0;

    if(rle == NULL)
        return 1;

    ptr_temp = &line_read[1];

    token = *ptr_temp;

    if(token == 'N')
    {
        *ptr_temp++;

        while(*ptr_temp == ' ') *ptr_temp++;

        unsigned int pattern_name_size = strlen(ptr_temp);

        rle->pattern_name = (char*)realloc(rle->pattern_name, pattern_name_size);

        strncpy(rle->pattern_name, ptr_temp, pattern_name_size);
    }

    return 0;
}

int rle_size_rule_parser(rle_t *rle, char *line_read)
{
    char *ptr_temp = NULL;

    if(rle == NULL)
        return 1;

    if((ptr_temp = strstr(line_read, "x")) != NULL)
    {
        if((ptr_temp = strstr(ptr_temp, "=")) != NULL)
        {
            *ptr_temp++;

            while(*ptr_temp == ' ') *ptr_temp++;

            rle->size_x = strtol(ptr_temp, NULL, 10);
        }    
    }

    if((ptr_temp = strstr(line_read, "y")) != NULL)
    {
        if((ptr_temp = strstr(ptr_temp, "=")) != NULL)
        {
            *ptr_temp++;

            while(*ptr_temp == ' ') *ptr_temp++;

            rle->size_y = strtol(ptr_temp, NULL, 10);
        }    
    }

    if((ptr_temp = strstr(line_read, "rule")) != NULL)
    {
        if((ptr_temp = strstr(line_read, "=")) != NULL)
        {
            *ptr_temp++;

            while(*ptr_temp == ' ') *ptr_temp++;

            if(strstr(line_read, "B3/S23"))
            {
                rle->rule = B3_S23;
            }
        }    
    }

    return 0;
}

int rle_tokens_parser(rle_t *rle, char *line_read)
{
    char *ptr_start = NULL;
    char *ptr_end = NULL;

    if(rle == NULL)
        return 1;

    ptr_start = line_read;
    ptr_end = ptr_start;

    while(*ptr_start != '!')
    {
        /*
        if(((ptr_end = strstr(ptr_start, "b")) != NULL) || 
        ((ptr_end = strstr(ptr_start, "o")) != NULL) ||
        ((ptr_end = strstr(ptr_start, "$")) != NULL))
        */

       if(*ptr_start == 'b' || *ptr_start == 'o' || *ptr_start == '$')
       {
            rle_token_t token = {0};

            if((ptr_start - ptr_end) > 0)
            {
                token.repetition = strtol(ptr_end, &ptr_start, 10);
            }
            else
            {
                token.repetition = 1;
            }

            token.token = *ptr_start;
            rle_add_token(rle, &token);

            *ptr_start++;

            if(ptr_start == NULL) break;

            ptr_end = ptr_start;            
       }
       else if(*ptr_start >= '0' && *ptr_start <= '9')
       {
           *ptr_start++;

            if(ptr_start == NULL) break;
       }
       else /* Any control caracter, like 0x0D, 0x0A */
       {
           break;
       }
    }

    return 0;
}

int rle_add_token(rle_t *rle, rle_token_t *token)
{
    if(rle == NULL || token == NULL)
        return 1;

    rle->tokens_count++;
    
    rle->tokens = (rle_token_t*)realloc(rle->tokens, rle->tokens_count * sizeof(rle_token_t));
    
    memcpy(&rle->tokens[rle->tokens_count-1], token, sizeof(rle_token_t));

    return 0;
}