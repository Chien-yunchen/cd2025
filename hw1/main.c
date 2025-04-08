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

    int ch, c;
    char buffer[256];
    int index;
    TokenNode *token_list = NULL;

    while ((ch = fgetc(file)) != EOF) {
        if (is_space(ch)) continue;

        if (is_letter(ch)) {
            index = 0;
            buffer[index++] = ch;
            while ((c = fgetc(file)) != EOF && !is_space(c) && is_alnum_or_underscore(c)) {
                buffer[index++] = c;
            }
            buffer[index] = '\0';
            if (c != EOF) ungetc(c, file);

            const char *token;
            if (is_keyword(buffer, &token))
                append_token(&token_list, buffer, token);
            else
                append_token(&token_list, buffer, ID_TOKEN);

        } else if (is_digit(ch)) {
            index = 0;
            buffer[index++] = ch;
            while ((c = fgetc(file)) != EOF && !is_space(c) && is_digit(c)) {
                buffer[index++] = c;
            }
            buffer[index] = '\0';
            if (c != EOF) ungetc(c, file);
            append_token(&token_list, buffer, LITERAL_TOKEN);

        } else {
            switch (ch) {
                case '=':
                    c = fgetc(file);
                    if (c == '=') append_token(&token_list, "==", EQUAL_TOKEN);
                    else { append_token(&token_list, "=", ASSIGN_TOKEN); if (c != EOF) ungetc(c, file); }
                    break;
                case '>':
                    c = fgetc(file);
                    if (c == '=') append_token(&token_list, ">=", GREATEREQUAL_TOKEN);
                    else { append_token(&token_list, ">", GREATER_TOKEN); if (c != EOF) ungetc(c, file); }
                    break;
                case '<':
                    c = fgetc(file);
                    if (c == '=') append_token(&token_list, "<=", LESSEQUAL_TOKEN);
                    else { append_token(&token_list, "<", LESS_TOKEN); if (c != EOF) ungetc(c, file); }
                    break;
                case '+': append_token(&token_list, "+", PLUS_TOKEN); break;
                case '-': append_token(&token_list, "-", MINUS_TOKEN); break;
                case '(': append_token(&token_list, "(", LEFTPAREN_TOKEN); break;
                case ')': append_token(&token_list, ")", RIGHTPAREN_TOKEN); break;
                case '{': append_token(&token_list, "{", LEFTBRACE_TOKEN); break;
                case '}': append_token(&token_list, "}", RIGHTBRACE_TOKEN); break;
                case ';': append_token(&token_list, ";", SEMICOLON_TOKEN); break;
                default: break;
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
