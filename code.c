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

void addPath(graph *flights, pathlist *paths, int dur, int price)
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
        addPath(flights, paths, dur, price);
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

int getVertex(graph *flights, char *str)
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

int getDur(path *node)
{
    return node->duration;
}

int getPrice(path *node)
{
    return node->price;
}

void sortPaths(path *head, int (*data)(path *))
{
    path *i = head;
    path *j = head;
    path tmp;
    while (i != NULL)
    {
        while (j->next != NULL)
        {
            if ((*data)(j) > (*data)(j->next))
            {
                tmp.duration = j->next->duration;
                tmp.price = j->next->price;
                tmp.stops = j->next->stops;

                j->next->duration = j->duration;
                j->next->price = j->price;
                j->next->stops = j->stops;

                j->duration = tmp.duration;
                j->price = tmp.price;
                j->stops = tmp.stops;
            }
            j = j->next;
        }
        j = head;
        i = i->next;
    }
}

int main()
{
    char *str1 = (char *)malloc(sizeof(char) * 16);
    char *str2 = (char *)malloc(sizeof(char) * 16);

    printf("Enter file name for flight list: ");
    scanf("%s", str1);
    FILE *fp = fopen(str1, "r");

    graph *flights = (graph *)malloc(sizeof(graph));
    flights->num = 0;
    flights->list = (vertex **)malloc(sizeof(vertex *));

    int i = 0, hour, minute, price, v1, v2;
    do
    {
        fscanf(fp, "%s %s %d %d %d ", str1, str2, &hour, &minute, &price);
        v1 = getVertex(flights, str1);
        v2 = getVertex(flights, str2);
        addEdge(flights, v1, v2, hour * 60 + minute, price);
        i++;
    } while (!feof(fp));

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

    char *stoplist = (char *)malloc(sizeof(char) * 50);
    path *node;
    bool stopsUsed;
    int k;
    int src;
    int dest;

    pathlist *paths = (pathlist *)malloc(sizeof(pathlist));

    while (true)
    {

        printf("\n\nsource: ");
        scanf("%s", str1);
        printf("destination: ");
        scanf("%s", str2);
        printf("max stop count: ");
        scanf("%d", &k);

        for (i = 0; i < flights->num; i++)
        {
            flights->list[i]->visited = false;
        }

        src = getVertex(flights, str1);
        dest = getVertex(flights, str2);

        paths->count = 0;
        paths->head = NULL;

        DFS(flights, paths, src, dest, k, 0, 0);

        int choice;
        printf("sort by (1- duration, 2- price): ");
        scanf("%d", &choice);

        if (choice == 1)
        {
            sortPaths(paths->head, getDur);
        }
        else
        {
            sortPaths(paths->head, getPrice);
        }

        printf("save results to file (1- yes, 2- no): ");
        scanf("%d", &choice);
        printf("\n");

        if (choice == 1)
        {
            fp = fopen("results.txt", "w");
        }

        if (paths->count == 0)
        {
            printf("No results found\n");
        }
        else
        {
            printf("|Source         |Destination    |Hour   |Minute |Price  |Stop\n");
            printf("+---------------+---------------+-------+-------+-------+---------------\n");

            if (choice == 1)
            {
                fprintf(fp, "|Source         |Destination    |Hour   |Minute |Price  |Stop\n");
                fprintf(fp, "+---------------+---------------+-------+-------+-------+---------------\n");
            }

            for (node = paths->head; node != NULL; node = node->next)
            {
                strcpy(stoplist, "");
                node->stops[src] = false;
                stopsUsed = false;
                for (i = 0; i < flights->num; i++)
                {
                    if (node->stops[i])
                    {
                        strcat(stoplist, flights->list[i]->name);
                        strcat(stoplist, ", ");
                        stopsUsed = true;
                    }
                }

                if (stopsUsed == false)
                {
                    strcpy(stoplist, "None");
                }
                else
                {
                    stoplist[strlen(stoplist) - 2] = '\0';
                }

                printf("|%-15s|%-15s", flights->list[src]->name, flights->list[dest]->name);
                printf("|%-7d|%-7d|%-7d|%s\n", node->duration / 60, node->duration % 60, node->price, stoplist);

                if (choice == 1)
                {
                    fprintf(fp, "|%-15s|%-15s", flights->list[src]->name, flights->list[dest]->name);
                    fprintf(fp, "|%-7d|%-7d|%-7d|%s\n", node->duration / 60, node->duration % 60, node->price, stoplist);
                }
            }
            fclose(fp);
        }
    }
}