#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdint.h> // For uint32_t

// Define the structure for a key-value pair
typedef struct Node {
    uint32_t key;      // Hexadecimal key
    uint32_t value;    // Hexadecimal value
    struct Node *next; // Pointer to the next node
} Node;

// Define the structure for the hash map
typedef struct HashMap {
    Node **buckets;    // Array of pointers to nodes
    int size;          // Number of buckets
} HashMap;

// Function declarations
unsigned int hash(uint32_t key, int size);
HashMap *createHashMap(int size);
void insert(HashMap *map, uint32_t key, uint32_t value);
uint32_t get(HashMap *map, uint32_t key);
uint32_t getHashKey(HashMap *map, uint32_t value);
void removeKey(HashMap *map, uint32_t key);
void freeHashMap(HashMap *map);

#endif // HASHMAP_H
