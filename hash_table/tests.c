#include "generic.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

int main()
{
    printf("Тест: базовые операции");
    HashTable *table = createHashTable(sizeof(int), sizeof(int));
    assert(table != NULL);

    int key = 1, val = 100;
    setItemHashTable(table, &key, &val, HashInt, intEquals);

    int *res = getItemHashTable(table, &key, HashInt, intEquals);
    assert(res && *res == 100);
    printf("нормас\n");

    printf("Тест: коллизии  ");
    int key1 = 1, key2 = 11; 
    int val1 = 10, val2 = 20;
    setItemHashTable(table, &key1, &val1, HashInt, intEquals);
    setItemHashTable(table, &key2, &val2, HashInt, intEquals);

    int *r1 = getItemHashTable(table, &key1, HashInt, intEquals);
    int *r2 = getItemHashTable(table, &key2, HashInt, intEquals);
    assert(r1 && *r1 == 10);
    assert(r2 && *r2 == 20);
    printf("нормас\n");

    printf("Тест: удаление и повторная вставка");
    int *deleted = popItemHashTable(table, &key1, HashInt, intEquals);
    assert(deleted && *deleted == 10);
    free(deleted);

    assert(getItemHashTable(table, &key1, HashInt, intEquals) == NULL);

    setItemHashTable(table, &key1, &val1, HashInt, intEquals);
    int *r3 = getItemHashTable(table, &key1, HashInt, intEquals);
    assert(r3 && *r3 == 10);
    printf("нормас\n");

    printf("Тест: угловые случаи и некорректные данные");
    assert(getItemHashTable(NULL, &key, HashInt, intEquals) == NULL);
    assert(getItemHashTable(table, NULL, HashInt, intEquals) == NULL);
    assert(popItemHashTable(NULL, &key, HashInt, intEquals) == NULL);
    assert(popItemHashTable(table, NULL, HashInt, intEquals) == NULL);
    setItemHashTable(NULL, &key, &val, HashInt, intEquals);
    setItemHashTable(table, NULL, &val, HashInt, intEquals);
    setItemHashTable(table, &key, NULL, HashInt, intEquals);
    printf("OK\n");

    freeHashTable(table);

    printf("ура-победа\n");
    return 0;
}
