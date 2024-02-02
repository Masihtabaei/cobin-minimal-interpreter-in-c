#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Global Variables
typedef enum
{
    INTEGER, PLUS, MINUS, STAR, SLASH, DOUBLE_STAR, LEFT_PARANTHESIS, RIGHT_PARANTHESIS, STATEMENT, COMMAND, IDENTIFIER, EOL
}Token_Type;

typedef struct
{
    Token_Type type;
    int value;
    char name[100];
}Token;

typedef struct
{
    char name[50];
    int value;
    void (*function_pointer) ();
}Symbol;

Symbol symbol_table[100];
int character_index;
int size_in_use;
char input_characters[999];
char name[50];
char name_character[2];
char current_character;
Token current_token;

// Function Prototypes
Token get_next_token(void);
int evaluate_expression(void);
int calculate_pow(int base, int exponent);
void initialize();


int main()
{
    while(1)
    {
        initialize();
        character_index = 0;
        size_in_use = 0;
        printf(">> ");
        fgets(input_characters, 999, stdin);
        current_character = input_characters[character_index];
        current_token = get_next_token();
        if(current_token.type == COMMAND)
        {
            for(int i = 0; i < sizeof symbol_table; i++)
            {
                if(!strcmp(current_token.name, symbol_table[i].name))
                {
                    symbol_table[i].function_pointer();
                    break;
                }
            }
        }else
        {
            printf("Result: %d\n", evaluate_expression());
        }
    }
    return 0;
}

void info_handler()
{
    printf("Info Handler successfully tested!\n");
}

void help_handler()
{
    printf("Help Handler successfully tested!\n");
}
void initialize()
{
    memset(name, 0, sizeof name);
    memset(name_character, 0, sizeof name_character);
    Symbol info;
    strncpy(info.name, "info", sizeof info.name);
    info.function_pointer = info_handler;
    Symbol help;
    strncpy(help.name, "help", sizeof help.name);
    help.function_pointer = help_handler;
    symbol_table[0] = info;
    symbol_table[1] = help;

}
void advance()
{
    character_index++;
    current_character = input_characters[character_index];
}
void skip_whitespaces()
{
    while(isspace(current_character))
    {
        advance();
    }
}

Token get_next_token(void)
{
    Token token_to_return;
    skip_whitespaces();
    if(isdigit(current_character))
    {
        int value = (int) (current_character - 48);
        advance();
        while(isdigit(current_character))
        {
            value = 10 * value + (int) (current_character - 48);
            advance();
        }
        token_to_return.type = INTEGER;
        token_to_return.value = value;
        return token_to_return;
    }
    if(current_character == '+')
    {
        token_to_return.type = PLUS;
        advance();
        return token_to_return;
    }
    if(current_character == '-')
    {
        token_to_return.type = MINUS;
        advance();
        return token_to_return;
    }
    if(current_character == '*')
    {
        token_to_return.type = STAR;
        advance();
        if(current_character == '*')
        {
            token_to_return.type = DOUBLE_STAR;
            advance();
        }
        return token_to_return;
    }
    if(current_character == '/')
    {
        token_to_return.type = SLASH;
        advance();
        return token_to_return;
    }
    if(current_character == '(')
    {
        token_to_return.type = LEFT_PARANTHESIS;
        advance();
        return token_to_return;
    }
    if(current_character == ')')
    {
        token_to_return.type = RIGHT_PARANTHESIS;
        advance();
        return token_to_return;
    }
    if(isalpha(current_character))
    {
        name_character[0] = current_character;
        strncat(name, name_character, 1);
        advance();
        while(isalpha(current_character))
        {
            name_character[0] = current_character;
            strncat(name, name_character, 1);
            advance();
        }
        token_to_return.type = COMMAND;
        memcpy(token_to_return.name, name, sizeof token_to_return.name);
        return token_to_return;

    }
    token_to_return.type = EOL;
    return token_to_return;

}

void match(Token_Type expected_type)
{
    if(current_token.type == expected_type)
    {
        current_token = get_next_token();
    }else
    {
        printf("Expected token: %d, got %d\n", expected_type, current_token.type);
    }
}

int evaluate_exponential()
{
    Token factor = current_token;
    if(factor.type == INTEGER){
        match(INTEGER);
        return factor.value;
    }
    else if(factor.type == LEFT_PARANTHESIS)
    {
        match(LEFT_PARANTHESIS);
        int result = evaluate_expression();
        match(RIGHT_PARANTHESIS);
        return result;
    }
}

int evaluate_factor()
{
    int first_exponential = evaluate_exponential();
    while(current_token.type == DOUBLE_STAR)
    {
        match(DOUBLE_STAR);
        first_exponential = calculate_pow(first_exponential, evaluate_exponential());
    }
    return first_exponential;
}

int evaluate_term(void)
{
    int first_factor = evaluate_factor();
    Token tail;
    while(current_token.type == STAR || current_token.type == SLASH)
    {
        tail = current_token;
        if(tail.type == STAR)
        {
            match(STAR);
            first_factor = first_factor * evaluate_term();
        }
        else if(tail.type == SLASH)
        {
            match(SLASH);
            first_factor = first_factor / evaluate_term();
        }
    }

    return first_factor;

}

int evaluate_expression()
{
    int first_term = evaluate_term();
    Token tail;
    while(current_token.type == PLUS || current_token.type == MINUS)
    {
        tail = current_token;
        if(tail.type == PLUS)
        {
            match(PLUS);
            first_term = first_term + evaluate_term();
        }
        else if(tail.type == MINUS)
        {
            match(MINUS);
            first_term = first_term - evaluate_term();
        }
    }
    return first_term;
}

int calculate_pow(int base, int exponent)
{
    int result = 1;
    for(int i = 0; i < exponent; i++)
    {
        result *= base;
    }
    return result;
}

//int evalute_expression()
//{
//    current_token = get_next_token();
//    Token left_side = current_token;
//    match(INTEGER);
//
//    Token middle = current_token;
//    if(middle.type == PLUS)
//    {
//        match(PLUS);
//    }
//    if(middle.type == MINUS)
//    {
//        match(MINUS);
//    }
//    if(middle.type == STAR)
//    {
//        match(STAR);
//    }
//    if(middle.type == SLASH)
//    {
//        match(SLASH);
//    }
//
//    Token right_side = current_token;
//    match(INTEGER);
//
//    if(middle.type == PLUS) return left_side.value + right_side.value;
//    if(middle.type == MINUS) return left_side.value - right_side.value;
//    if(middle.type == STAR) return left_side.value * right_side.value;
//    if(middle.type == SLASH) return left_side / right_side.value;
//
//}
//
//void test_get_next_token()
//{
//     while(1)
//    {
//        character_index = 0;
//        size_in_use = 0;
//        printf("> ");
//        fgets(input_characters, 999, stdin);
//        int test  = get_next_token().type;
//        switch(test)
//        {
//            case 0: printf("INTEGER\n"); break;
//            case 1: printf("PLUS\n"); break;
//            case 2: printf("EOL\n"); break;
//        }
//
//    }
//
//}
