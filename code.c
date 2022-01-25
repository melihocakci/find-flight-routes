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

// listeye yeni rota ekler
void addPath(graph *flights, pathlist *paths, int dur, int price)
{
    path *newPath = (path *)malloc(sizeof(path)); // yeni rota
    newPath->duration = dur - 60;
    newPath->price = price;
    newPath->stops = malloc(sizeof(bool) * flights->num);
    int i;
    for (i = 0; i < flights->num; i++)
    {
        newPath->stops[i] = flights->list[i]->visited; // rotadaki duraklari listele
    }

    // rotayi listeye ekle
    newPath->next = paths->head;
    paths->head = newPath;
    paths->count++;
}

// depth first search algoritmasi
void DFS(graph *flights, pathlist *paths, int cur, int dest, int k, int dur, int price)
{
    if (cur == dest) // bitis noktasini kontrol et
    {
        addPath(flights, paths, dur, price); // rota ekle
    }
    else if (k != -1) // fonksiyonu yeniden cagir
    {
        flights->list[cur]->visited = true; // ziyaret edildi

        edge *node;
        for (node = flights->list[cur]->head; node != NULL; node = node->next)
        {
            if (!flights->list[node->index]->visited)
            {
                // recursive cagri
                DFS(flights, paths, node->index, dest, k - 1, dur + node->duration + 60, price + node->price);
            }
        }

        flights->list[cur]->visited = false; // ziyaret edilmedi
    }
}

// noktanin indisini dondurur. nokta yok ise once ekler
int getVertex(graph *flights, char *str)
{
    int i;
    for (i = 0; i < flights->num; i++) // noktalari kontrol et
    {
        if (strcmp(flights->list[i]->name, str) == 0)
        {
            return i;
        }
    }

    // listede yok ise yeni nokta olustur
    flights->num++;
    flights->list = realloc(flights->list, sizeof(vertex *) * flights->num);
    vertex *newVertex = (vertex *)malloc(sizeof(vertex));
    strcpy(newVertex->name, str);
    newVertex->head = NULL;
    flights->list[flights->num - 1] = newVertex;
    return flights->num - 1;
}

// grafa kenar ekle
void addEdge(graph *flights, int v1, int v2, int dur, int price)
{
    edge *newEdge; // yeni kenar

    // ilk noktaya kenar ekle
    newEdge = (edge *)malloc(sizeof(edge));
    newEdge->index = v2;
    newEdge->duration = dur;
    newEdge->price = price;

    newEdge->next = flights->list[v1]->head;
    flights->list[v1]->head = newEdge;

    // ikinci noktaya kenar ekle
    newEdge = (edge *)malloc(sizeof(edge));
    newEdge->index = v1;
    newEdge->duration = dur;
    newEdge->price = price;

    newEdge->next = flights->list[v2]->head;
    flights->list[v2]->head = newEdge;
}

// rotadan siralama icin alinacak degeri dondurur
int getDur(path *node)
{
    return node->duration;
}

// rotadan siralama icin alinacak degeri dondurur
int getPrice(path *node)
{
    return node->price;
}

// rotalari siralayan bubble sort fonksiyonu
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
    char *str1 = (char *)malloc(sizeof(char) * 16); // string 1
    char *str2 = (char *)malloc(sizeof(char) * 16); // string 2

    printf("Enter file name for flight list: ");
    scanf("%s", str1);
    FILE *fp = fopen(str1, "r"); // bilgilerin okunacagi dosya

    graph *flights = (graph *)malloc(sizeof(graph)); // grafin tutulacagi degisken
    flights->num = 0;
    flights->list = (vertex **)malloc(sizeof(vertex *));

    // dosyadan okuma
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

    // grafi ekrana yazdirma
    edge *edges;
    for (i = 0; i < flights->num; i++)
    {
        printf("\n%d %s", i, flights->list[i]->name);
        for (edges = flights->list[i]->head; edges != NULL; edges = edges->next)
        {
            printf(" -> %s", flights->list[edges->index]->name);
        }
    }

    char *stoplist = (char *)malloc(sizeof(char) * 50); // duraklari tutan string
    path *node;                                         // rotalara bakmak icin degisken
    bool stopsUsed;                                     // durak kullanilma kontrolu
    int k;                                              // durak sayisi
    int src;                                            // baslangic noktasi
    int dest;                                           // bitis noktasi

    pathlist *paths = (pathlist *)malloc(sizeof(pathlist));

    while (true)
    {

        printf("\n\nsource: ");
        scanf("%s", str1); // baslangic sehir ismi
        printf("destination: ");
        scanf("%s", str2); // bitis sehir ismi
        printf("max stop count: ");
        scanf("%d", &k); // durak sayisi

        for (i = 0; i < flights->num; i++)
        {
            flights->list[i]->visited = false;
        }

        src = getVertex(flights, str1);  // baslangic
        dest = getVertex(flights, str2); // bitis

        paths->count = 0;
        paths->head = NULL;

        DFS(flights, paths, src, dest, k, 0, 0); // fonksiyon cagrisi

        int choice; // kullanici tercihleri
        printf("sort by (1- duration, 2- price): ");
        scanf("%d", &choice);

        if (choice == 1) // tercihe gore siralama
        {
            sortPaths(paths->head, getDur);
        }
        else
        {
            sortPaths(paths->head, getPrice);
        }

        printf("save results to file (1- yes, 2- no): ");
        scanf("%d", &choice); // dosyaya yazdirma tercihi
        printf("\n");

        if (choice == 1)
        {
            fp = fopen("results.txt", "w"); // cikis dosyasi
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
                // sonuclarin yazdirilmasi
                printf("|%-15s|%-15s", flights->list[src]->name, flights->list[dest]->name);
                printf("|%-7d|%-7d|%-7d|%s\n", node->duration / 60, node->duration % 60, node->price, stoplist);

                if (choice == 1)
                {
                    // sonuclarin dosyaya yazdirilmasi
                    fprintf(fp, "|%-15s|%-15s", flights->list[src]->name, flights->list[dest]->name);
                    fprintf(fp, "|%-7d|%-7d|%-7d|%s\n", node->duration / 60, node->duration % 60, node->price, stoplist);
                }
            }
            fclose(fp);
        }
    }
}