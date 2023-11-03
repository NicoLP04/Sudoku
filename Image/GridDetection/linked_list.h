#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
    void* value;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct List
{
    Node* head;
    Node* tail;
    size_t length;
} List;

Node* initNode(void* val);

void appendNode(List* list, Node* node);

void appendValue(List* list, void* value);

void freeNode(Node* node);

void freeList(List* list);

void printList(List* list);

void printListOfLines(List* List);

#endif
