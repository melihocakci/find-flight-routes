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
    struct path *next;
} path;

typedef struct pathlist
{
    int count;
    struct path *head;
} pathlist;

void add_path(graph *flights, pathlist *paths, int dur, int price)
{
    path *newPath = (path *)malloc(sizeof(path));
    newPath->duration = dur - 60;
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

void DFS(graph *flights, pathlist *paths, int cur, int dest, int k, int dur, int price)
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
                DFS(flights, paths, node->index, dest, k - 1, dur + node->duration + 60, price + node->price);
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
    strcpy(newVertex->name, str);
    newVertex->head = NULL;
    flights->list[flights->num - 1] = newVertex;
    return flights->num - 1;
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
    fclose(fp);

    edge *edges;
    for (i = 0; i < flights->num; i++)
    {
        printf("\n%d %s", i, flights->list[i]->name);
        for (edges = flights->list[i]->head; edges != NULL; edges = edges->next)
        {
            printf(" -> %s", flights->list[edges->index]->name);
        }
    }
    printf("\n\n");

    for (i = 0; i < flights->num; i++)
    {
        flights->list[i]->visited = false;
    }

    pathlist *paths = (pathlist *)malloc(sizeof(pathlist));
    paths->count = 0;
    paths->head = NULL;

    int src = 0;
    int dest = 4;
    int k = 20;

    DFS(flights, paths, src, dest, k, 0, 0);

    char *stoplist = (char *)malloc(sizeof(char) * 50);

    if (paths->count == 0)
    {
        printf("No results found");
        return 0;
    }

    printf("|Source\t\t|Destination\t|Hour\t|Minute\t|Price\t|Stop\n");
    printf("+---------------+---------------+-------+-------+-------+------------------------\n");

    path *node;
    bool stops;
    for (node = paths->head; node != NULL; node = node->next)
    {
        strcpy(stoplist, "");
        node->stops[src] = false;
        stops = false;
        for (i = 0; i < flights->num; i++)
        {
            if (node->stops[i])
            {
                strcat(stoplist, flights->list[i]->name);
                strcat(stoplist, ", ");
                stops = true;
            }
        }

        if (!stops)
        {
            strcpy(stoplist, "None");
        }
        else
        {
            stoplist[strlen(stoplist) - 2] = '\0';
        }

        printf("|%-15s|%-15s", flights->list[src]->name, flights->list[dest]->name);
        printf("|%d\t|%d\t|%d\t|%s\n", node->duration / 60, node->duration % 60, node->price, stoplist);
    }

    free(stoplist);
}