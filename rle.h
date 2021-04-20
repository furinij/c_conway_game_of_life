#ifndef RLE_H_INCLUDED
#define RLE_H_INCLUDED

typedef enum rle_rule_en_t
{
    NONE,
    B3_S23,
    B36_S23,
} rle_rule_en_t;

typedef struct rle_token_t
{
    unsigned int repetition;
    char token;
} rle_token_t;

typedef struct rle_t
{
    char *pattern_name;
    unsigned int size_x;
    unsigned int size_y;
    rle_rule_en_t rule;
    unsigned int tokens_count;
    rle_token_t *tokens;
} rle_t;

int rle_get(rle_t *rle, char *filename);
int rle_init(rle_t *rle);
int rle_deinit(rle_t *rle);
int rle_message_parser(rle_t *rle, char *line_read);
int rle_hash_parser(rle_t *rle, char *line_read);
int rle_size_rule_parser(rle_t *rle, char *line_read);
int rle_tokens_parser(rle_t *rle, char *line_read);
int rle_add_token(rle_t *rle, rle_token_t *token);

#endif /* RLE_H_INCLUDED */