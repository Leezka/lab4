#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../../lab3/vector/generic.h"
#include "generic.h"

int HashInt(const void *key)
{
    // TODO: реализовать
}

int HashString(const void *key)
{
    // TODO: реализовать
}

HashTable *createHashTable(size_t key_size, size_t val_size)
{
    // TODO: реализовать
}

void setItemHashTable(HashTable *table, void *key, void *data, HashFunc hash, CmpFunc cmp)
{
    // TODO: реализовать
}

void rehashHashTable(HashTable *table, HashFunc hash, CmpFunc cmp)
{
    // TODO: реализовать
}

void *getItemHashTable(HashTable *table, void *key, HashFunc hash, CmpFunc cmp)
{
    // TODO: реализовать
}

void *popItemHashTable(HashTable *table, void *key, HashFunc hash, CmpFunc cmp)
{
    // TODO: реализовать
}

unsigned long int getCollisionCount(HashTable *table, HashFunc hash)
{
    // TODO: реализовать
}

void freeHashTable(HashTable *table)
{
    // TODO: реализовать
}