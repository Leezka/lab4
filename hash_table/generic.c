#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../../lab3/vector/generic.h"
#include "generic.h"

#define LOAD_FACTOR 0.5

//всякое вспомогательное
static unsigned char* get_flag(HashTable* table, size_t index) {
    unsigned char** slot_ptr = getVectorItem(table->values, index);
    return (slot_ptr && *slot_ptr) ? *slot_ptr : NULL;
}

static void* get_key(HashTable* table, size_t index) {
    unsigned char* slot = get_flag(table, index);
    return slot ? slot + 1 : NULL;
}

static void* get_value(HashTable* table, size_t index) {
    unsigned char* slot = get_flag(table, index);
    return slot ? slot + 1 + table->key_size : NULL;
}

int HashInt(const void *key) {
    if (!key) return 0;
    int k = *(int*)key;
    if (k < 0) k = -k;
    double A = 0.6180339887; // метод умножения
    double frac = k * A - (int)(k * A);
    return (int)(frac * 1000);
}

int HashString(const void *key) {
    if (!key) return 0;
    const char* str = (const char*)key;
    unsigned long hash = 0;
    int c;
    while ((c = *str++)) hash = hash * 31 + c;
    double A = 0.6180339887;
    double frac = hash * A - (unsigned long)(hash * A);
    return (int)(frac * 1000);
}

HashTable *createHashTable(size_t key_size, size_t val_size) {
    if (key_size == 0 || val_size == 0) return NULL;

    HashTable *table = malloc(sizeof(HashTable));
    if (!table) return NULL;

    table->key_size = key_size;
    table->val_size = val_size;
    table->size = 0;
    table->capacity = TABLE_MIN_SIZE;
    table->values = createVector(sizeof(unsigned char*));
    if (!table->values) {
        free(table);
        return NULL;
    }

    for (size_t i = 0; i < table->capacity; i++) {
        unsigned char* slot = malloc(1 + key_size + val_size);
        if (!slot) {
            freeHashTable(table);
            return NULL;
        }
        slot[0] = SLOT_EMPTY;
        memset(slot + 1, 0, key_size + val_size);
        appendVectorItem(table->values, &slot);
    }

    return table;
}

void setItemHashTable(HashTable *table, void *key, void *data,
                      HashFunc hash, CmpFunc cmp) {
    if (!table || !key || !data || !hash || !cmp) return;

    if ((double)table->size / table->capacity > LOAD_FACTOR) {
        rehashHashTable(table, hash, cmp);
    }

    int h = hash(key);
    for (size_t i = 0; i < table->capacity; i++) {
        size_t idx = (h + i) % table->capacity; 
        unsigned char* flag = get_flag(table, idx);
        if (!flag) continue;

        void* k = get_key(table, idx);
        void* v = get_value(table, idx);

        if (*flag == SLOT_EMPTY || *flag == SLOT_DELETED) {
            *flag = SLOT_OCCUPIED;
            memcpy(k, key, table->key_size);
            memcpy(v, data, table->val_size);
            table->size++;
            return;
        }

        if (*flag == SLOT_OCCUPIED && cmp(k, key)) {
            memcpy(v, data, table->val_size);
            return;
        }
    }
}

void *getItemHashTable(HashTable *table, void *key, HashFunc hash, CmpFunc cmp) {
    if (!table || !key || !hash || !cmp) return NULL;

    int h = hash(key);
    for (size_t i = 0; i < table->capacity; i++) {
        size_t idx = (h + i) % table->capacity;
        unsigned char* flag = get_flag(table, idx);
        if (!flag) continue;

        if (*flag == SLOT_EMPTY) return NULL;

        if (*flag == SLOT_OCCUPIED) {
            void* k = get_key(table, idx);
            if (k && cmp(k, key)) return get_value(table, idx);
        }
    }
    return NULL;
}

void *popItemHashTable(HashTable *table, void *key, HashFunc hash, CmpFunc cmp) {
    if (!table || !key || !hash || !cmp) return NULL;

    int h = hash(key);
    for (size_t i = 0; i < table->capacity; i++) {
        size_t idx = (h + i) % table->capacity;
        unsigned char* flag = get_flag(table, idx);
        if (!flag) continue;

        if (*flag == SLOT_EMPTY) return NULL;

        if (*flag == SLOT_OCCUPIED) {
            void* k = get_key(table, idx);
            if (k && cmp(k, key)) {
                void* v = get_value(table, idx);
                void* copy = malloc(table->val_size);
                if (!copy) return NULL;
                memcpy(copy, v, table->val_size);
                *flag = SLOT_DELETED;
                table->size--;
                return copy;
            }
        }
    }
    return NULL;
}

void rehashHashTable(HashTable *table, HashFunc hash, CmpFunc cmp) {
    if (!table || !hash || !cmp) return;

    size_t old_capacity = table->capacity;
    Vector* old_values = table->values;

    table->capacity *= 2;
    table->size = 0;
    table->values = createVector(sizeof(unsigned char*));
    if (!table->values) {
        table->values = old_values;
        table->capacity = old_capacity;
        return;
    }

    for (size_t i = 0; i < table->capacity; i++) {
        unsigned char* slot = malloc(1 + table->key_size + table->val_size);
        if (!slot) continue;
        slot[0] = SLOT_EMPTY;
        memset(slot + 1, 0, table->key_size + table->val_size);
        appendVectorItem(table->values, &slot);
    }

    for (size_t i = 0; i < old_capacity; i++) {
        unsigned char** slot_ptr = getVectorItem(old_values, i);
        if (!slot_ptr || !*slot_ptr) continue;
        unsigned char* slot = *slot_ptr;
        if (slot[0] == SLOT_OCCUPIED) {
            void* key = slot + 1;
            void* val = key + table->key_size;
            setItemHashTable(table, key, val, hash, cmp);
        }
        free(slot);
    }
    vectorFree(old_values);
}

unsigned long int getCollisionCount(HashTable *table, HashFunc hash) {
    if (!table || !hash) return 0;

    unsigned long collisions = 0;
    for (size_t i = 0; i < table->capacity; i++) {
        unsigned char* flag = get_flag(table, i);
        if (flag && *flag == SLOT_OCCUPIED) {
            void* k = get_key(table, i);
            if (!k) continue;
            size_t ideal = hash(k) % table->capacity;
            if (ideal != i) collisions++;
        }
    }
    return collisions;
}

void freeHashTable(HashTable *table) {
    if (!table) return;
    if (table->values) {
        for (size_t i = 0; i < table->capacity; i++) {
            unsigned char** slot_ptr = getVectorItem(table->values, i);
            if (slot_ptr && *slot_ptr) free(*slot_ptr);
        }
        vectorFree(table->values);
    }
    free(table);
}
