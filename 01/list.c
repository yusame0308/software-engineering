#include <stdio.h>
#include <stdlib.h>
struct list {
    struct list *next;
    int data;
};
int main() {
    struct list *head;
    head = malloc(sizeof(struct list));
    head = NULL;

    for (int i = 100; i > 0; i--) {
        struct list *p;
        p = malloc(sizeof(struct list));
        p->data = i;
        p->next = head;
        head = p;
    }

    while (head != NULL) {
        printf("%d ", head->data);
        head = head->next;
    }

    exit(0);
}