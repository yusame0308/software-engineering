#include <stdio.h>
#include <stdlib.h>

#define SIZE 10000

void insertion_sort(int list[SIZE]) {
    for (int j = 1; j < SIZE; j++) {
        int key = list[j];
        int i = j-1;
        while (i >= 0 && list[i] > key) {
            list[i+1] = list[i];
            i--;
        }
        list[i+1] = key;
    }
}

// 配列の要素を最大100個出力
void printList(int list[SIZE]) {
    int max = SIZE > 100 ? 100 : SIZE;
    for (int i = 0; i < max; i++) {
        printf("%d ", list[i]);
    }
    printf("\n");
}

int main() {
    // ランダムの数値配列を作成
    int list[SIZE];
    for (int i = 0; i < SIZE; i++) {
        list[i] = rand() % (SIZE*2);
    }

    printf("Before sorting\n");
    printList(list);

    insertion_sort(list);

    printf("After sorting\n");
    printList(list);

    exit(0);
}