#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

struct Node {
    int vertex;
    int weight;  
    struct Node* next;
};

struct Graph {
    int V;
    struct Node** adjList;
};

//create a new node 
struct Node* createNode(int v, int w) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->vertex = v;
    newNode->weight = w;
    newNode->next = NULL;
    return newNode;
}


// create a graph with a given number of vertices
struct Graph* createGraph(int vertices) {
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
    graph->V = vertices;
    graph->adjList = (struct Node**)malloc(vertices * sizeof(struct Node*));
    
    for (int i = 0; i < vertices; i++) {
        graph->adjList[i] = NULL;
    }
    return graph;
}

// add an edge to the graph
void addEdge(struct Graph* graph, int src, int dest, int weight) {
    struct Node* newNode = createNode(dest, weight);
    newNode->next = graph->adjList[src];
    graph->adjList[src] = newNode;
}

// show graphic 
void display(struct Graph* graph) {
    for (int i = 0; i < graph->V; i++) {
        printf("%c: ", 'a' + i);  
        struct Node* temp = graph->adjList[i];
        while (temp) {
            printf("%c(%d) ", 'a' + temp->vertex, temp->weight);
            temp = temp->next;
        }
        printf("\n");
    }
}

// DFS search
void DFSSearch(struct Graph* graph, int start) {
    bool* visited = (bool*)calloc(graph->V, sizeof(bool));
    int* stack = (int*)malloc(graph->V * sizeof(int));
    int top = -1;
    
    stack[++top] = start;
    
    while (top >= 0) {
        int vertex = stack[top--];
        
        if (!visited[vertex]) {
            visited[vertex] = true;
            printf("%d ", vertex);
            
            struct Node* temp = graph->adjList[vertex];
            while (temp) {
                if (!visited[temp->vertex]) {
                    stack[++top] = temp->vertex;
                }
                temp = temp->next;
            }
        }
    }
    
    free(visited);
    free(stack);
}

// BFS search
void BFSSearch(struct Graph* graph, int start) {
    bool* visited = (bool*)calloc(graph->V, sizeof(bool));
    int* queue = (int*)malloc(graph->V * sizeof(int));
    int front = 0, rear = 0;
    
    visited[start] = true;
    queue[rear++] = start;
    
    while (front < rear) {
        int vertex = queue[front++];
        printf("%d ", vertex);
        
        struct Node* temp = graph->adjList[vertex];
        while (temp) {
            if (!visited[temp->vertex]) {
                visited[temp->vertex] = true;
                queue[rear++] = temp->vertex;
            }
            temp = temp->next;
        }
    }
    
    free(visited);
    free(queue);
}

// delete a node from the graph
void DeleteNode(struct Graph* graph, int node) {
    // delete all edges to the node
    for (int i = 0; i < graph->V; i++) {
        if (i == node) continue;
        
        struct Node* prev = NULL;
        struct Node* curr = graph->adjList[i];
        
        while (curr) {
            if (curr->vertex == node) {
                if (prev) {
                    prev->next = curr->next;
                } else {
                    graph->adjList[i] = curr->next;
                }
                struct Node* toDelete = curr;
                curr = curr->next;
                free(toDelete);
            } else {
                prev = curr;
                curr = curr->next;
            }
        }
    }
    
    // delete the node itself
    struct Node* curr = graph->adjList[node];
    while (curr) {
        struct Node* toDelete = curr;
        curr = curr->next;
        free(toDelete);
    }
    graph->adjList[node] = NULL;
}

// calculate the degree of each vertex
void CalcuDegree(struct Graph* graph) {
    // out degree
    for (int i = 0; i < graph->V; i++) {
        int outDegree = 0;
        struct Node* temp = graph->adjList[i];
        while (temp) {
            outDegree++;
            temp = temp->next;
        }
        printf("Vertex %d has outdegree: %d\n", i, outDegree);
    }
    
    // in degree
    int* inDegree = (int*)calloc(graph->V, sizeof(int));
    for (int i = 0; i < graph->V; i++) {
        struct Node* temp = graph->adjList[i];
        while (temp) {
            inDegree[temp->vertex]++;
            temp = temp->next;
        }
    }
    
    for (int i = 0; i < graph->V; i++) {
        printf("Vertex %d has indegree: %d\n", i, inDegree[i]);
    }
    
    free(inDegree);
}

// Prim MST algorithm to find the minimum spanning tree
void PrimMST(struct Graph* graph) {
    bool* inMST = (bool*)calloc(graph->V, sizeof(bool));
    int* parent = (int*)malloc(graph->V * sizeof(int));
    int* key = (int*)malloc(graph->V * sizeof(int));
    
    for (int i = 0; i < graph->V; i++) {
        key[i] = INT_MAX;
        parent[i] = -1;
    }
    
    key[0] = 0;
    
    for (int count = 0; count < graph->V - 1; count++) {
        int minKey = INT_MAX;
        int u = -1;
        
        for (int v = 0; v < graph->V; v++) {
            if (!inMST[v] && key[v] < minKey) {
                minKey = key[v];
                u = v;
            }
        }
        
        if (u == -1) break;
        
        inMST[u] = true;
        
        struct Node* temp = graph->adjList[u];
        while (temp) {
            int v = temp->vertex;
            if (!inMST[v] && temp->weight < key[v]) {
                parent[v] = u;
                key[v] = temp->weight;
            }
            temp = temp->next;
        }
    }
    
    printf("the edge of minimun spanning tree\n");
    for (int i = 1; i < graph->V; i++) {
        if (parent[i] != -1) {
            printf("%c - %c (%d)\n", 'a' + parent[i], 'a' + i, key[i]);
        }
    }
    
    free(inMST);
    free(parent);
    free(key);
}


// free the graph and its nodes
void freeGraph(struct Graph* graph) {
    for (int i = 0; i < graph->V; i++) {
        struct Node* current = graph->adjList[i];
        while (current) {
            struct Node* next = current->next;
            free(current);
            current = next;
        }
    }
    free(graph->adjList);
    free(graph);
}

int main() {
    struct Graph* g = createGraph(9);  
    
    addEdge(g, 0, 1, 2);   // a->b
    addEdge(g, 0, 5, 9);   // a->f
    addEdge(g, 0, 6, 15);  // a->g
    addEdge(g, 1, 2, 4);   // b->c
    addEdge(g, 1, 6, 6);   // b->g
    addEdge(g, 2, 3, 2);   // c->d
    addEdge(g, 2, 8, 15);  // c->i
    addEdge(g, 3, 8, 1);   // d->i
    addEdge(g, 3, 4, 1);   // d->e
    addEdge(g, 5, 7, 11);  // f->h
    addEdge(g, 5, 4, 6);   // f->e
    addEdge(g, 6, 7, 15);  // g->h
    addEdge(g, 6, 8, 2);   // g->i
    addEdge(g, 7, 4, 3);   // h->e
    addEdge(g, 7, 8, 4);   // h->i
    
    printf("adjacency list representation of the graph:\n");
    display(g);
    
    printf("\ncaulating degrees of vertices:\n");
    CalcuDegree(g);
    
    printf("\nDFSSearch");
    DFSSearch(g, 0);
    printf("\n");
    
    printf("\nBFSSearch");
    BFSSearch(g, 0);
    printf("\n");
    
    printf("\nthe minimum spanning tree\n");
    PrimMST(g);
    
    freeGraph(g);
    return 0;
}