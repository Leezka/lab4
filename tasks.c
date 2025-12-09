#include "tasks.h"
#include "hash_table/generic.h"
#include "../lab3/vector/generic.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int HashStringPtr(const void *key)
{
    const char *str = *(const char **)key;
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return (int)(hash & 0x7FFFFFFF);
}

int CmpStringPtr(const void *a, const void *b)
{
    const char *str1 = *(const char **)a;
    const char *str2 = *(const char **)b;
    return strcmp(str1, str2) == 0;
}

Vector *removeDuplicates(Vector *strings)
{
    if (!strings) return NULL;
    Vector *result = createVector(sizeof(char *));
    HashTable *table = createHashTable(sizeof(char *), sizeof(char *));
    for (size_t i = 0; i < strings->size; i++) {
        char *str = *(char **)getVectorItem(strings, i);
        if (!getItemHashTable(table, &str, HashStringPtr, CmpStringPtr)) {
            setItemHashTable(table, &str, &str, HashStringPtr, CmpStringPtr);
            appendVectorItem(result, &str);
        }
    }
    freeHashTable(table);
    return result;
}

Vector *encodeStrings(Vector *strings)
{
    if (!strings) return NULL;
    Vector *result = createVector(sizeof(int));
    HashTable *table = createHashTable(sizeof(char *), sizeof(int));
    int code = 0;
    for (size_t i = 0; i < strings->size; i++) {
        char *str = *(char **)getVectorItem(strings, i);
        int *existing = getItemHashTable(table, &str, HashStringPtr, CmpStringPtr);
        if (existing) {
            appendVectorItem(result, existing);
        } else {
            int *new_code = (int *)malloc(sizeof(int));
            *new_code = code++;
            setItemHashTable(table, &str, new_code, HashStringPtr, CmpStringPtr);
            appendVectorItem(result, new_code);
        }
    }
    freeHashTable(table);
    return result;
}

HashTable *swapKeysValues(HashTable *table, HashFunc hash, CmpFunc cmp)
{
    if (!table) return NULL;
    HashTable *newTable = createHashTable(table->val_size, table->key_size);
    for (size_t i = 0; i < table->values->size; i++) {
        unsigned char *slot = (unsigned char *)getVectorItem(table->values, i);
        if (!slot) continue;
        if (slot[0] == SLOT_OCCUPIED) {
            void *key = slot + 1 + table->key_size;
            void *val = slot + 1;
            setItemHashTable(newTable, key, val, hash, cmp);
        }
    }
    return newTable;
}
