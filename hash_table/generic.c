#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../../lab3/vector/generic.h"
#include "generic.h"
#define LOAD_FACTOR 0.5
//вспомогательная история: 
static size_t slotSize(HashTable *table) {
    return 1 + table->key_size + table->val_size;
}

static void *getSlot(HashTable *table, size_t index) {
    return getVectorItem(table->values, index);
}

static unsigned char *slotFlag(HashTable *table, size_t index) {
    return (unsigned char *)getSlot(table, index);
}

static void *slotKey(HashTable *table, size_t index) {
    return (char *)getSlot(table, index) + 1;
}

static void *slotValue(HashTable *table, size_t index) {
    return (char *)getSlot(table, index) + 1 + table->key_size;
}

int HashInt(const void *key)
{
    return *(int *)key;
}

int HashString(const void *key)
{
    const unsigned char *str = key;
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return (int)hash;
}

HashTable *createHashTable(size_t key_size, size_t val_size)
{
    HashTable *table = malloc(sizeof(HashTable));
    if (!table) return NULL;

    table->key_size = key_size;
    table->val_size = val_size;
    table->size = 0;
    table->capacity = TABLE_MIN_SIZE;

    table->values = createVector(1 + key_size + val_size);
    if (!table->values) {
        free(table);
        return NULL;
    }

    unsigned char flag = SLOT_EMPTY;
    for (size_t i = 0; i < table->capacity; i++) {
        appendVectorItem(table->values, &flag);
        void *slot = getVectorItem(table->values, i);
        memset((char *)slot + 1, 0, key_size + val_size);
    }

    return table;
}

void setItemHashTable(HashTable *table, void *key, void *data, HashFunc hash, CmpFunc cmp)
{
    if (!table || !key || !data) return;
    if ((double)table->size / table->capacity > LOAD_FACTOR) {
        rehashHashTable(table, hash, cmp);
    }
    size_t h = hash(key);
    size_t index;
    size_t i = 0;
    size_t slot_count = table->capacity;
    size_t first_deleted = slot_count;
    while (i < slot_count) {
        index = (h + i * i) % slot_count;
        unsigned char *f = (unsigned char *)getVectorItem(table->values, index);
        void *k = (char *)getVectorItem(table->values, index) + 1;
        if (*f == SLOT_OCCUPIED && cmp(k, key)) {
            void *v = (char *)k + table->key_size;
            memcpy(v, data, table->val_size);
            return;
        }
        if (*f == SLOT_DELETED && first_deleted == slot_count) first_deleted = index;
        if (*f == SLOT_EMPTY) break;
        i++;
    }
    size_t insert_index = (first_deleted != slot_count) ? first_deleted : index;
    unsigned char *f = (unsigned char *)getVectorItem(table->values, insert_index);
    *f = SLOT_OCCUPIED;
    void *k = (char *)getVectorItem(table->values, insert_index) + 1;
    void *v = (char *)k + table->key_size;
    memcpy(k, key, table->key_size);
    memcpy(v, data, table->val_size);
    table->size++;
}

void rehashHashTable(HashTable *table, HashFunc hash, CmpFunc cmp)
{
    size_t old_capacity = table->capacity;
    size_t new_capacity = old_capacity * 2;

    HashTable new_table;
    new_table.key_size = table->key_size;
    new_table.val_size = table->val_size;
    new_table.size = 0;
    new_table.capacity = new_capacity;
    new_table.values = createVector(1 + table->key_size + table->val_size);
    unsigned char flag = SLOT_EMPTY;
    for (size_t i = 0; i < new_capacity; i++) {
        appendVectorItem(new_table.values, &flag);
        void *slot = getVectorItem(new_table.values, i);
        memset((char *)slot + 1, 0, table->key_size + table->val_size);
    }
    for (size_t i = 0; i < old_capacity; i++) {
        unsigned char *f = (unsigned char *)getVectorItem(table->values, i);
        if (*f == SLOT_OCCUPIED) {
            void *k = (char *)getVectorItem(table->values, i) + 1;
            void *v = (char *)getVectorItem(table->values, i) + 1 + table->key_size;
            setItemHashTable(&new_table, k, v, hash, cmp);
        }
    }
    vectorFree(table->values);
    table->values = new_table.values;
    table->capacity = new_capacity;
}

void *getItemHashTable(HashTable *table, void *key, HashFunc hash, CmpFunc cmp)
{
    if (!table || !key) return NULL;
    size_t h = hash(key);
    size_t index;
    size_t i = 0;
    size_t slot_count = table->capacity;
    while (i < slot_count) {
        index = (h + i * i) % slot_count;
        unsigned char *f = (unsigned char *)getVectorItem(table->values, index);
        if (*f == SLOT_EMPTY) return NULL;
        void *k = (char *)getVectorItem(table->values, index) + 1;
        if (*f == SLOT_OCCUPIED && cmp(k, key)) {
            return (char *)k + table->key_size;
        }
        i++;
    }
    return NULL;
}

void *popItemHashTable(HashTable *table, void *key, HashFunc hash, CmpFunc cmp)
{
    if (!table || !key) return NULL;

    size_t h = hash(key);
    size_t index;
    size_t i = 0;
    size_t slot_count = table->capacity;

    while (i < slot_count) {
        index = (h + i * i) % slot_count;
        unsigned char *f = (unsigned char *)getVectorItem(table->values, index);
        void *k = (char *)getVectorItem(table->values, index) + 1;
        void *v = (char *)k + table->key_size;

        if (*f == SLOT_EMPTY) return NULL;
        if (*f == SLOT_OCCUPIED && cmp(k, key)) {
            void *value_copy = malloc(table->val_size);
            memcpy(value_copy, v, table->val_size);
            *f = SLOT_DELETED;
            table->size--;
            return value_copy;
        }
        i++;
    }

    return NULL;
}

unsigned long int getCollisionCount(HashTable *table, HashFunc hash)
{
    if (!table) return 0;

    unsigned long int collisions = 0;
    for (size_t i = 0; i < table->capacity; i++) {
        unsigned char *f = (unsigned char *)getVectorItem(table->values, i);
        if (*f == SLOT_OCCUPIED) {
            void *k = (char *)getVectorItem(table->values, i) + 1;
            size_t ideal = hash(k) % table->capacity;
            if (ideal != i) collisions++;
        }
    }
    return collisions;
}

void freeHashTable(HashTable *table)
{
    if (!table) return;
    if (table->values) vectorFree(table->values);
    free(table);
}