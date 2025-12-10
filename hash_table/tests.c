#include "generic.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cmp_int(const void* a, const void* b) {
    return *(int*)a == *(int*)b;
}

void test_empty_table() {
    printf("Тест пустой таблицы: ");
    HashTable* table = createHashTable(sizeof(int), sizeof(int));
    assert(table != NULL);
    
    int key = 5;
    void* result = getItemHashTable(table, &key, HashInt, cmp_int);
    assert(result == NULL);
    
    result = popItemHashTable(table, &key, HashInt, cmp_int);
    assert(result == NULL);
    
    freeHashTable(table);
    printf("пройден\n");
}

void test_minimal_data() {
    printf("Тест минимальных данных: ");
    HashTable* table = createHashTable(sizeof(int), sizeof(int));
    assert(table != NULL);
    
    int key = 42;
    int value = 100;
    
    setItemHashTable(table, &key, &value, HashInt, cmp_int);
    assert(table->size == 1);
    
    int* res = getItemHashTable(table, &key, HashInt, cmp_int);
    assert(res != NULL);
    assert(*res == 100);
    
    freeHashTable(table);
    printf("пройден\n");
}

void test_collisions() {
    printf("Тест коллизий: ");
    HashTable* table = createHashTable(sizeof(int), sizeof(int));
    assert(table != NULL);
    
    int keys[] = {1, 17, 33, 49, 65};
    for (int i = 0; i < 5; i++) {
        int value = i * 100;
        setItemHashTable(table, &keys[i], &value, HashInt, cmp_int);
    }
    
    assert(table->size == 5);
    
    for (int i = 0; i < 5; i++) {
        int* res = getItemHashTable(table, &keys[i], HashInt, cmp_int);
        assert(res != NULL);
        assert(*res == i * 100);
    }
    
    freeHashTable(table);
    printf("пройден\n");
}

void test_rehash() {
    printf("Тест переполнения таблицы: ");
    HashTable* table = createHashTable(sizeof(int), sizeof(int));
    assert(table != NULL);
    
    int initial_capacity = table->capacity;
    
    for (int i = 0; i < 7; i++) {
        setItemHashTable(table, &i, &i, HashInt, cmp_int);
    }
    
    assert(table->capacity > initial_capacity);
    assert(table->size == 7);
    
    for (int i = 0; i < 7; i++) {
        int* res = getItemHashTable(table, &i, HashInt, cmp_int);
        assert(res != NULL);
        assert(*res == i);
    }
    
    freeHashTable(table);
    printf("пройден\n");
}

void test_delete_and_reinsert() {
    printf("Тест удаления и повторной вставки: ");
    HashTable* table = createHashTable(sizeof(int), sizeof(int));
    assert(table != NULL);
    
    int key = 10;
    int value1 = 100;
    int value2 = 200;
    
    setItemHashTable(table, &key, &value1, HashInt, cmp_int);
    assert(table->size == 1);
    
    int* popped = popItemHashTable(table, &key, HashInt, cmp_int);
    assert(popped != NULL);
    assert(*popped == 100);
    free(popped);
    assert(table->size == 0);
    
    void* res = getItemHashTable(table, &key, HashInt, cmp_int);
    assert(res == NULL);
    
    setItemHashTable(table, &key, &value2, HashInt, cmp_int);
    assert(table->size == 1);
    
    int* new_res = getItemHashTable(table, &key, HashInt, cmp_int);
    assert(new_res != NULL);
    assert(*new_res == 200);
    
    freeHashTable(table);
    printf("пройден\n");
}

void test_invalid_arguments() {
    printf("Тест некорректных данных: ");
    HashTable* table = createHashTable(sizeof(int), sizeof(int));
    assert(table != NULL);
    unsigned long collisions = getCollisionCount(NULL, HashInt);
    assert(collisions == 0);
    collisions = getCollisionCount(table, HashInt);
    freeHashTable(NULL);  
    freeHashTable(table);
    
    printf("пройден\n");
}

void test_typical_scenarios() {
    printf("Тест типичных сценариев: ");
    HashTable* table = createHashTable(sizeof(int), sizeof(int));
    assert(table != NULL);
    
    for (int i = 0; i < 5; i++) {
        int value = i * 10;
        setItemHashTable(table, &i, &value, HashInt, cmp_int);
    }
    assert(table->size == 5);
    
    for (int i = 0; i < 5; i++) {
        int* res = getItemHashTable(table, &i, HashInt, cmp_int);
        assert(res != NULL);
        assert(*res == i * 10);
    }
    
    int key_to_remove = 2;
    int* popped = popItemHashTable(table, &key_to_remove, HashInt, cmp_int);
    assert(popped != NULL);
    free(popped);
    assert(table->size == 4);
    
    void* res = getItemHashTable(table, &key_to_remove, HashInt, cmp_int);
    assert(res == NULL);
    
    int new_value = 999;
    setItemHashTable(table, &key_to_remove, &new_value, HashInt, cmp_int);
    assert(table->size == 5);
    
    int* new_res = getItemHashTable(table, &key_to_remove, HashInt, cmp_int);
    assert(new_res != NULL);
    assert(*new_res == 999);
    
    int key_to_update = 3;
    int updated_value = 777;
    setItemHashTable(table, &key_to_update, &updated_value, HashInt, cmp_int);
    assert(table->size == 5);
    
    int* updated_res = getItemHashTable(table, &key_to_update, HashInt, cmp_int);
    assert(updated_res != NULL);
    assert(*updated_res == 777);
    
    freeHashTable(table);
    printf("пройден\n");
}

int main() {
    printf("Тесты\n\n");
    
    test_empty_table();
    test_minimal_data();
    test_collisions();
    test_rehash();
    test_delete_and_reinsert();
    test_invalid_arguments();
    test_typical_scenarios();
    
    printf("\nУра-победа\n");
    return 0;
}