#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node{
    char character;
    int count;
    struct Node *next;
} Node;

// 尋找節點
Node *find_or_add(Node **head, char ch){
    Node *current = *head;
    Node *prev = NULL;
    while (current) {
        if (current->character == ch){
            current->count++;
            return current;
        }
        prev = current;
        current = current->next;
    }
    // 新增節點
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->character = ch;
    new_node->count = 1;
    new_node->next = NULL;
    if (prev)
        prev->next = new_node;
    else
        *head = new_node;
    return new_node;
}

void free_list(Node *head){
    while (head){
        Node *temp = head;
        head = head->next;
        free(temp);
    }
}

int main(){
    FILE *file = fopen(__FILE__, "r"); // 讀取程式碼
    if (!file){
        perror("Can't open file");
        return 1;
    }
    
    Node *head = NULL;
    char ch;
    while ((ch = fgetc(file)) != EOF){
        // 篩掉中文、tab(\t)、enter(\n)
        if ((unsigned char)ch < 128 && ch != '\t' && ch != '\n'){
            find_or_add(&head, ch);
        }
    }
    fclose(file);
    
    // 輸出
    Node *current = head;
    while (current){
        printf("%c : %d\n", current->character, current->count);
        current = current->next;
    }
    
    free_list(head);
    return 0;
}
