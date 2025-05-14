#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LITERAL_TOKEN        "LITERAL_TOKEN"
#define LEFTPAREN_TOKEN      "LEFTPAREN_TOKEN"
#define RIGHTPAREN_TOKEN     "RIGHTPAREN_TOKEN"
#define PLUS_TOKEN           "PLUS_TOKEN"

typedef struct TokenNode {
    char value[256];
    char type[32];
    struct TokenNode *next;
} TokenNode;

TokenNode *current_token;
char *token_type;

// ==== Scanner functions ====
int is_digit(char ch) {
    return (ch >= '0' && ch <= '9');
}

int is_space(char ch) {
    return (ch == ' ' || ch == '\n' || ch == '\t');
}

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

void advance() {
    if (current_token != NULL) {
        current_token = current_token->next;
        token_type = (current_token != NULL) ? current_token->type : NULL;
    } else {
        token_type = NULL;
    }
}

// ==== Parser functions ====
int parse_S();
int parse_S_Prime(int left);
int parse_E();

int parse_S() {
    printf("S -> E S'\n");
    switch (token_type != NULL ? token_type[0] : '\0') {
        case 'L':  // LITERAL_TOKEN
        case 'L' + 1:  // not real, but keep compiler happy
            if (strcmp(token_type, LITERAL_TOKEN) == 0) {
                int val = parse_E();
                return parse_S_Prime(val);
            }
            // fall through
        case '(':
            if (strcmp(token_type, LEFTPAREN_TOKEN) == 0) {
                int val = parse_E();
                return parse_S_Prime(val);
            }
            // fall through
        default:
            printf("Error: unexpected token in S\n");
            exit(1);
    }
}

int parse_S_Prime(int left) {
    if (token_type == NULL) {
        printf("S' -> epsilon\n");
        return left;
    }

    if (strcmp(token_type, PLUS_TOKEN) == 0) {
        printf("S' -> + S\n");
        advance();
        int right = parse_S();
        return left + right;
    } else {
        printf("S' -> epsilon\n");
        return left;
    }
}

int parse_E() {
    printf("E -> ");
    if (token_type == NULL) {
        printf("Error: unexpected end of input in E\n");
        exit(1);
    }

    if (strcmp(token_type, LITERAL_TOKEN) == 0) {
        printf("%s\n", current_token->value);
        int val = atoi(current_token->value);
        advance();
        return val;
    } else if (strcmp(token_type, LEFTPAREN_TOKEN) == 0) {
        printf("( S )\n");
        advance();
        int val = parse_S();
        if (token_type == NULL || strcmp(token_type, RIGHTPAREN_TOKEN) != 0) {
            printf("Error: Expected ')' in E\n");
            exit(1);
        }
        advance();
        return val;
    } else {
        printf("Error: unexpected token %s in E\n", current_token->value);
        exit(1);
    }
}

// ==== Main ====
int main() {
    FILE *file = fopen("input.txt", "r");
    if (!file) {
        perror("Cannot open input.txt");
        return 1;
    }

    TokenNode *token_list = NULL;
    enum State { START, NUMBER } state = START;
    int ch;
    char buffer[256];
    int index = 0;

    // Scanner
    while ((ch = fgetc(file)) != EOF) {
        switch (state) {
            case START:
                if (is_space(ch)) continue;
                else if (is_digit(ch)) {
                    buffer[index++] = ch;
                    state = NUMBER;
                } else {
                    char symbol[2] = {ch, '\0'};
                    const char *type = NULL;
                    if (ch == '+') type = PLUS_TOKEN;
                    else if (ch == '(') type = LEFTPAREN_TOKEN;
                    else if (ch == ')') type = RIGHTPAREN_TOKEN;
                    if (type) append_token(&token_list, symbol, type);
                    else {
                        printf("Error: unknown symbol %c\n", ch);
                        exit(1);
                    }
                }
                break;

            case NUMBER:
                if (is_digit(ch)) {
                    buffer[index++] = ch;
                } else {
                    buffer[index] = '\0';
                    append_token(&token_list, buffer, LITERAL_TOKEN);
                    index = 0;
                    state = START;
                    ungetc(ch, file);
                }
                break;
        }
    }
    if (state == NUMBER && index > 0) {
        buffer[index] = '\0';
        append_token(&token_list, buffer, LITERAL_TOKEN);
    }
    fclose(file);

    // Parser
    current_token = token_list;
    token_type = (current_token != NULL) ? current_token->type : NULL;

    int result = parse_S();

    if (current_token != NULL) {
        printf("Error: Extra tokens after parsing\n");
        return 1;
    }

    printf("Result: %d\n", result);

    // free token list
    TokenNode *curr = token_list;
    while (curr != NULL) {
        TokenNode *temp = curr;
        curr = curr->next;
        free(temp);
    }

    return 0;
}
