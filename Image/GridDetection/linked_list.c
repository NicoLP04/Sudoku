#include "linked_list.h"
#include "houghtransform.h"


// initialization functions


Node* initNode(void* val)
{
    Node* node = (Node*)malloc(sizeof(Node));

    if (node == NULL)
        errx(EXIT_FAILURE, "initNode : malloc failed!");

    node->value = val;
    node->next = NULL;
    node->prev = NULL;

    return node;
}


// insertion functions


void appendNode(List* list, Node* node)
{
    if (list->head == NULL)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
    }

    list->length++;
}


void appendValue(List* list, void* value)
{
    Node* node = initNode(value);

    if (list->head == NULL)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
    }
    
    list->length++;
}


// memory freeing functions

void freeNode(Node* node)
{
    node->prev = NULL;
    node->next = NULL;
    free(node->value);
    free(node);
}


void freeList(List* list)
{
    Node* curr = list->head;

    while (curr->next != NULL)
    {
        Node* next = curr->next;
        free(curr);
        curr = next;
    }
}


// printing functions

void printList(List* list)
{
    Node* curr = list->head;

    printf("[");

    while(curr->next != NULL)
    {
        printf("  %i,", *((int *)curr->value));
        curr = curr->next;
    }

    printf("  %i ]\n",*((int *)curr->value));
}

void printListOfLines(List* list)
{
    Node* curr = list->head;

    printf("[\n");

    while(curr->next != NULL)
    {
        printf(" %5i ;", ((Line *)curr->value)->X0);
        printf(" %5i ;", ((Line *)curr->value)->Y0);
        printf(" %5i ;", ((Line *)curr->value)->X1);
        printf(" %5i\n", ((Line *)curr->value)->Y1);
        curr = curr->next;
    }

    printf(" %5i ;", ((Line *)curr->value)->X0);
    printf(" %5i ;", ((Line *)curr->value)->Y0);
    printf(" %5i ;", ((Line *)curr->value)->X1);
    printf(" %5i\n", ((Line *)curr->value)->Y1);

    printf("]\n");
}
