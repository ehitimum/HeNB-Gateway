#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>

// Hash function to calculate the index
unsigned int hash(uint32_t key, int size) {
    return key % size; // Simple modulus hash
}

// Create a new hash map
HashMap *createHashMap(int size) {
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    map->buckets = (Node **)calloc(size, sizeof(Node *));
    map->size = size;
    return map;
}

// Insert a key-value pair into the hash map
void insert(HashMap *map, uint32_t key, uint32_t value) {
    unsigned int index = hash(key, map->size);
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->key = key;
    newNode->value = value;
    newNode->next = map->buckets[index];
    map->buckets[index] = newNode;
}

// Retrieve a value by key
uint32_t get(HashMap *map, uint32_t key) {
    unsigned int index = hash(key, map->size);
    Node *current = map->buckets[index];
    while (current) {
        if (current->key == key) {
            return current->value;
        }
        current = current->next;
    }
    return 0; // Key not found
}

uint32_t getHashKey(HashMap *map, uint32_t value) {
    for (int i = 0; i < map->size; i++) { // Iterate through all buckets
        Node *current = map->buckets[i];
        while (current) { // Iterate through the linked list in each bucket
            if (current->value == value) { // Check if the value matches
                return current->key; // Return the corresponding key
            }
            current = current->next;
        }
    }
    return 0; // Value not found
}

// Remove a key-value pair from the hash map
void removeKey(HashMap *map, uint32_t key) {
    unsigned int index = hash(key, map->size);
    Node *current = map->buckets[index];
    Node *prev = NULL;

    while (current) {
        if (current->key == key) {
            if (prev) {
                prev->next = current->next;
            } else {
                map->buckets[index] = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Free the hash map
void freeHashMap(HashMap *map) {
    for (int i = 0; i < map->size; i++) {
        Node *current = map->buckets[i];
        while (current) {
            Node *temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(map->buckets);
    free(map);
}
