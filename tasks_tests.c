#include "tasks.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../lab3/vector/generic.h"

// TODO: реализовать

int main() {
    HashTable *table = createHashTable(sizeof(int), sizeof(int));

    // 1. Пустая таблица
    int key = 10;
    assert(getItemHashTable(table, &key, HashInt, intEquals) == NULL);
    assert(popItemHashTable(table, &key, HashInt, intEquals) == NULL);

    // 2. Минимальные данные
    int val = 100;
    setItemHashTable(table, &key, &val, HashInt, intEquals);
    int *res = getItemHashTable(table, &key, HashInt, intEquals);
    assert(res && *res == 100);

    // 3. Коллизии
    int key2 = key + table->capacity; 
    int val2 = 200;
    setItemHashTable(table, &key2, &val2, HashInt, intEquals);
    int *res2 = getItemHashTable(table, &key2, HashInt, intEquals);
    assert(res2 && *res2 == 200);

    // 4. Переполнение таблицы
    for (int i = 0; i < 20; i++) {
        int k = i + 1000;
        int v = i * 10;
        setItemHashTable(table, &k, &v, HashInt, intEquals);
    }
    assert(table->size > 0);

    // 5. Удаление и повторная вставка
    int *pop_val = popItemHashTable(table, &key, HashInt, intEquals);
    assert(pop_val && *pop_val == 100);
    free(pop_val);
    val = 300;
    setItemHashTable(table, &key, &val, HashInt, intEquals);
    res = getItemHashTable(table, &key, HashInt, intEquals);
    assert(res && *res == 300);

    // 6. Некорректные данные
    assert(getItemHashTable(NULL, &key, HashInt, intEquals) == NULL);
    assert(popItemHashTable(NULL, &key, HashInt, intEquals) == NULL);
    setItemHashTable(NULL, &key, &val, HashInt, intEquals);

    // 7. Типичные сценарии
    for (int i = 0; i < 10; i++) {
        int k = i;
        int v = i * 5;
        setItemHashTable(table, &k, &v, HashInt, intEquals);
    }
    for (int i = 0; i < 10; i++) {
        int k = i;
        int *v = getItemHashTable(table, &k, HashInt, intEquals);
        assert(v && *v == i * 5);
    }

    freeHashTable(table);
    printf("Ура-победа\n");
    return 0;
}