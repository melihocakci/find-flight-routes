#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct edge
{
    int index;
    int duration;
    int price;
    struct edge *next;
} edge;

typedef struct vertex
{
    int index;
    char name[16];
    bool visited;
    struct edge *head;
} vertex;

typedef struct graph
{
    int num;
    struct vertex **list;
} graph;

typedef struct path
{
    bool *stops;
    int duration;
    int price;
    path *next;
} path;

typedef struct pathlist
{
    int count;
    path *head;
} pathlist;

void add_path(graph *flights, pathlist *paths, int dur, int price)
{
    path *newPath = (path *)malloc(sizeof(path));
    newPath->duration = dur;
    newPath->price = price;
    newPath->stops = malloc(sizeof(bool) * flights->num);
    int i;
    for (i = 0; i < flights->num; i++)
    {
        newPath->stops[i] = flights->list[i]->visited;
    }

    newPath->next = paths->head;
    paths->head = newPath;
    paths->count++;
}

void DFS(graph *flights, pathlist *paths, int k, int cur, int dest, int dur, int price)
{
    if (cur == dest)
    {
        add_path(flights, paths, dur, price);
    }
    else if (k != -1)
    {
        flights->list[cur]->visited = true;

        edge *node;
        for (node = flights->list[cur]->head; node != NULL; node = node->next)
        {
            if (!flights->list[node->index]->visited)
            {
                DFS(flights, paths, k - 1, node->index, dest, dur + node->duration + 1, price + node->price);
            }
        }

        flights->list[cur]->visited = false;
    }
}

int add_or_find_vertex(graph *flights, char *str)
{
    int i;
    for (i = 0; i < flights->num; i++)
    {
        if (strcmp(flights->list[i]->name, str) == 0)
        {
            return i;
        }
    }

    flights->num++;
    flights->list = realloc(flights->list, sizeof(vertex *) * flights->num);
    vertex *newVertex = (vertex *)malloc(sizeof(vertex));
    newVertex->index = flights->num - 1;
    strcpy(newVertex->name, str);
    newVertex->head = NULL;
    flights->list[newVertex->index] = newVertex;
    return newVertex->index;
}

void addEdge(graph *flights, int v1, int v2, int dur, int price)
{
    edge *newEdge;
    newEdge = (edge *)malloc(sizeof(edge));
    newEdge->index = v2;
    newEdge->duration = dur;
    newEdge->price = price;

    newEdge->next = flights->list[v1]->head;
    flights->list[v1]->head = newEdge;

    newEdge = (edge *)malloc(sizeof(edge));
    newEdge->index = v1;
    newEdge->duration = dur;
    newEdge->price = price;

    newEdge->next = flights->list[v2]->head;
    flights->list[v2]->head = newEdge;
}

int main()
{
    graph *flights = (graph *)malloc(sizeof(graph));
    FILE *fp = fopen("sample.txt", "r");

    flights->num = 0;
    flights->list = (vertex **)malloc(sizeof(vertex *));

    char str1[16], str2[16];
    int i, hour, minute, price, v1, v2;
    for (i = 0; i < 8; i++)
    {
        fscanf(fp, "%s %s %d %d %d", str1, str2, &hour, &minute, &price);
        v1 = add_or_find_vertex(flights, str1);
        v2 = add_or_find_vertex(flights, str2);
        addEdge(flights, v1, v2, hour * 60 + minute, price);
    }

    edge *iter;
    for (i = 0; i < flights->num; i++)
    {
        printf("\n%d %s", i, flights->list[i]->name);
        iter = iter = flights->list[i]->head;
        while (iter != NULL)
        {
            printf(" -> %d %d %d", iter->index, iter->duration, iter->price);
            iter = iter->next;
        }
    }

    fclose(fp);
}