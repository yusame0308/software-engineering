#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SIZE 10000

int binary_search(int key, int nums[SIZE]) {
    int j, k, p, i;
    j = 0; k = SIZE-1; i = 0;
    while (j <= k) {
        i += 1;
        p = ceil((j+k)/2);
        if (key < nums[p]) k = p-1;
        else if (key > nums[p]) j = p+1;
        else if (key == nums[p]) return i;
    }
    return -1;
}

int hash(int key, int nums[SIZE]) {
    const int delta = 3;
    const int bucket_size = SIZE * 2; // 衝突する可能性が低くなるように大きめのサイズを用意
    int table[bucket_size] = {}; // 初期値0で定義

    // ハッシュテーブルを作成
    for (int i = 0; i < SIZE; i++) {
        int index = nums[i] % bucket_size;
        while (table[index] != 0) {
            index = (index + delta) % bucket_size;
        }
        table[index] = nums[i];
    }

    // keyを探す
    int count = 1;
    int index = key % bucket_size;
    while (table[index] != key && count < bucket_size) {
        index = (index + delta) % bucket_size;
        count++;
    }

    return count == bucket_size ? -1 : count; // keyが見つからなかったら-1を返す
}

int main() {
    // 対象の配列を作成
    int nums[SIZE];
    nums[0] = 1;
    for (int i = 1; i < SIZE; i++) {
        nums[i] = nums[i-1] + rand() % 3 + 1;
    }
    // 末尾50要素を出力
    for (int i = SIZE - 51; i < SIZE; i++) {
        printf("%d ", nums[i]);
    }

    // ユーザからの入力
    int key;
    printf("\nKey? ");
    scanf("%d", &key);

    // Binary search
    int binary_index = binary_search(key, nums);
    if (binary_index == -1) {
        printf("Binary Search: Not Found");
    } else {
        printf("Binary Search: Found (Step = %d)", binary_index);
    }

    printf("\n");

    // Hash
    int hash_index = hash(key, nums);
    if (hash_index == -1) {
        printf("Hash: Not Found");
    } else {
        printf("Hash: Found (Step = %d)", hash_index);
    }

    printf("\n");
    exit(0);
}