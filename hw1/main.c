#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TYPE_TOKEN           "TYPE_TOKEN"
#define MAIN_TOKEN           "MAIN_TOKEN"
#define IF_TOKEN             "IF_TOKEN"
#define ELSE_TOKEN           "ELSE_TOKEN"
#define WHILE_TOKEN          "WHILE_TOKEN"
#define ID_TOKEN             "ID_TOKEN"
#define LITERAL_TOKEN        "LITERAL_TOKEN"
#define LEFTPAREN_TOKEN      "LEFTPAREN_TOKEN"
#define RIGHTPAREN_TOKEN     "RIGHTPAREN_TOKEN"
#define LEFTBRACE_TOKEN      "LEFTBRACE_TOKEN"
#define RIGHTBRACE_TOKEN     "RIGHTBRACE_TOKEN"
#define ASSIGN_TOKEN         "ASSIGN_TOKEN"
#define EQUAL_TOKEN          "EQUAL_TOKEN"
#define GREATER_TOKEN        "GREATER_TOKEN"
#define LESS_TOKEN           "LESS_TOKEN"
#define GREATEREQUAL_TOKEN   "GREATEREQUAL_TOKEN"
#define LESSEQUAL_TOKEN      "LESSEQUAL_TOKEN"
#define PLUS_TOKEN           "PLUS_TOKEN"
#define MINUS_TOKEN          "MINUS_TOKEN"
#define SEMICOLON_TOKEN      "SEMICOLON_TOKEN"

typedef struct TokenNode {
    char value[256];
    char type[32];
    struct TokenNode *next;
} TokenNode;

TokenNode* create_node(const char *value, const char *type) {
    TokenNode *node = (TokenNode*)malloc(sizeof(TokenNode));
    strcpy(node->value, value);
    strcpy(node->type, type);
    node->next = NULL;
    return node;
}

void append_token(TokenNode **head, const char *value, const char *type) {
    TokenNode *new_node = create_node(value, type);
    if (*head == NULL) {
        *head = new_node;
    } else {
        TokenNode *curr = *head;
        while (curr->next != NULL) curr = curr->next;
        curr->next = new_node;
    }
}

int is_letter(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

int is_digit(char ch) {
    return (ch >= '0' && ch <= '9');
}

int is_alnum_or_underscore(char ch) {
    return is_letter(ch) || is_digit(ch);
}

int is_space(char ch) {
    return ch == ' ' || ch == '\n' || ch == '\t';
}

int is_keyword(const char *str, const char **token) {
    if (strcmp(str, "int") == 0) *token = TYPE_TOKEN;
    else if (strcmp(str, "main") == 0) *token = MAIN_TOKEN;
    else if (strcmp(str, "if") == 0) *token = IF_TOKEN;
    else if (strcmp(str, "else") == 0) *token = ELSE_TOKEN;
    else if (strcmp(str, "while") == 0) *token = WHILE_TOKEN;
    else return 0;
    return 1;
}

int main() {
    FILE *file = fopen("input.c", "r");
    if (!file) {
        perror("Cannot open input file");
        return 1;
    }

    enum State { START, ID, NUMBER, ASSIGN } state = START;
    int ch;
    char buffer[256];
    int index = 0;
    TokenNode *token_list = NULL;

    while ((ch = fgetc(file)) != EOF) {
        switch (state) {
            case START:
                if (is_space(ch)) {
                    continue;
                } else if (is_letter(ch)) {
                    buffer[index++] = ch;
                    state = ID;
                } else if (is_digit(ch)) {
                    buffer[index++] = ch;
                    state = NUMBER;
                } else if (ch == '=') {
                    buffer[index++] = ch;
                    state = ASSIGN;
                } else {
                    char symbol[3] = {ch, '\0'};
                    const char *type = NULL;
                    switch (ch) {
                        case '+': type = PLUS_TOKEN; break;
                        case '-': type = MINUS_TOKEN; break;
                        case '(': type = LEFTPAREN_TOKEN; break;
                        case ')': type = RIGHTPAREN_TOKEN; break;
                        case '{': type = LEFTBRACE_TOKEN; break;
                        case '}': type = RIGHTBRACE_TOKEN; break;
                        case ';': type = SEMICOLON_TOKEN; break;
                        case '>': {
                            int next = fgetc(file);
                            if (next == '=') {
                                strcpy(symbol, ">=");
                                type = GREATEREQUAL_TOKEN;
                            } else {
                                ungetc(next, file);
                                type = GREATER_TOKEN;
                            }
                            break;
                        }
                        case '<': {
                            int next = fgetc(file);
                            if (next == '=') {
                                strcpy(symbol, "<=");
                                type = LESSEQUAL_TOKEN;
                            } else {
                                ungetc(next, file);
                                type = LESS_TOKEN;
                            }
                            break;
                        }
                        default: break;
                    }
                    if (type) append_token(&token_list, symbol, type);
                    state = START;
                }
                break;

            case ID: {
                if (is_alnum_or_underscore(ch)) {
                    buffer[index++] = ch;
                } else {
                    buffer[index] = '\0';
                    const char *token;
                    if (is_keyword(buffer, &token))
                        append_token(&token_list, buffer, token);
                    else
                        append_token(&token_list, buffer, ID_TOKEN);
                    ungetc(ch, file);
                    index = 0;
                    state = START;
                }
                break;
            }

            case NUMBER: {
                if (is_digit(ch)) {
                    buffer[index++] = ch;
                } else {
                    buffer[index] = '\0';
                    append_token(&token_list, buffer, LITERAL_TOKEN);
                    ungetc(ch, file);
                    index = 0;
                    state = START;
                }
                break;
            }

            case ASSIGN: {
                if (ch == '=') {
                    buffer[index++] = ch;
                    buffer[index] = '\0';
                    append_token(&token_list, buffer, EQUAL_TOKEN);
                } else {
                    buffer[index] = '\0';
                    append_token(&token_list, buffer, ASSIGN_TOKEN);
                    ungetc(ch, file);
                }
                index = 0;
                state = START;
                break;
            }
        }
    }

    fclose(file);

    TokenNode *curr = token_list;
    while (curr != NULL) {
        printf("%s: %s\n", curr->value, curr->type);
        TokenNode *temp = curr;
        curr = curr->next;
        free(temp);
    }

    return 0;
}
