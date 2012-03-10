#include "structs.h"

#define Vertex int
#define Edge Arc
#define EDGE ARC
#define graph digraph
#define Graph Digraph

#define maxV 100
#define maxN 128
#define DISTANCIA_NODES 1000

#define GRAPHinit DIGRAPHinit
#define GRAPHshow DIGRAPHshow

typedef enum {FALSE, TRUE} Boolean;

typedef struct
{
    Vertex v;
    Vertex w;
} Arc;

Arc ARC (Vertex v, Vertex w)
{
  Arc e;
  e.v= v;
  e.w= w;
  
  return e;
}

struct digraph
{
  int V;
  int A;
  int **adj;
};

typedef struct digraph *Digraph;

int **MATRIXint (int r, int c, int val)
{
  Vertex i, j;
  
  int **m = malloc(r * sizeof(int *));
  
  for (i = 0; i < r; i++)
    m[i] = malloc(c * sizeof(int));
  
  for (i = 0; i < r; i++)
    for (j = 0; j < c; j++)
      m[i][j] = val;
  
  return m;
}

Digraph DIGRAPHinit (int V)
{
  Digraph G = malloc(sizeof *G);
  
  G->V = V;
  G->A = 0;
  G->adj = MATRIXint(V, V, 0);
  
  return G;
}

void GRAPHterminate (Graph G)
{
  Vertex i;
  
  for (i = 0; i < G->V; i++) {
    free (G->adj[i]);
  }

  free (G);
}

void DIGRAPHinsertA (Digraph G, Vertex v, Vertex w)
{
  if(G->adj[v][w] == 0 && v != w) {
    G->adj[v][w] = 1;
    (G->A)++;
  }
}

void DIGRAPHremoveA (Digraph G, Vertex v, Vertex w)
{
  if(G->adj[v][w] == 1) {
    G->adj[v][w] = 0;
    (G->A)--;
  }
}

void GRAPHinsertE (Graph G, Vertex v, Vertex w)
{
  DIGRAPHinsertA (G, v, w);
  DIGRAPHinsertA (G, w, v);
}

void GRAPHremoveE (Graph G, Vertex v, Vertex w)
{
  DIGRAPHremoveA (G, v, w);
  DIGRAPHremoveA (G, w, v);
}

void DIGRAPHshow (Digraph G)
{
  int i, j;

  for (i = 0; i < G->V; i++) {
    printf ("%d: ", i);
    for (j = 0; j < G->V; j++)
      if (G->adj[i][j] == 1)
        printf ("%d ", j);
    printf ("\n");
  }
}

/*
   essa funcao conecta os vertices do grafo
   vou aproveita-la para criar os _nodes_
   e os _links_
   os nodes ja foram criados antes da chamada
   a essa funcao
*/
void GRAPHGridConnect (Graph G, int gridWidth, linkNode *Nodes,
                       linkNodeID *NodesBordaSuperior, linkNodeID *NodesBordaInferior,
                       linkNodeID *NodesBordaEsquerda, linkNodeID *NodesBordaDireita)
{
  int v, t;
  Boolean borda; /* indica se o vertice esta presente em uma borda do grafo */
  char *nome = NULL;
  int linha, coluna;
  int tipo_node;

  t = gridWidth * gridWidth;

  /* testa se e um vertice de borda */  
  /* test if the vertex is on one of the frames of the grid */
  printf("[GRAPHGridConnect]: conectando grafo");
  for (v = 1; v <= t; v++) 
  {
    borda = FALSE;
    printf("v: %d\n", v);
    if (v <= gridWidth) /* borda de cima */
    {
      borda = TRUE;
      *NodesBordaSuperior = newNodeID(v, *NodesBordaSuperior);
    }
    else
      GRAPHinsertE (G, v, v - gridWidth);

    if (v > t - gridWidth) /* borda de baixo */
    {
      borda = TRUE;
      *NodesBordaInferior = newNodeID(v, *NodesBordaInferior);
    }
    else
      GRAPHinsertE (G, v, v + gridWidth);

    if ( (v - 1) % gridWidth == 0 ) /* borda da esquerda */
    {
      borda = TRUE;
      *NodesBordaEsquerda = newNodeID(v, *NodesBordaEsquerda);
    }
    else
      GRAPHinsertE (G, v, v - 1);

    if ( v % gridWidth == 0 ) /* borda da direita */
    {
      borda = TRUE;
      *NodesBordaDireita = newNodeID(v, *NodesBordaDireita);
    }
    else
      GRAPHinsertE (G, v, v + 1);

    linha = t/gridWidth;
    coluna = t%gridWidth;

		/* ToDo: colocar o nome */
    /* strcpy(nome, "\0"); */

    if (borda)
      tipo_node = entry_exit;
    else
      tipo_node = intersection;

    Nodes[v] = newNode (v,
                        tipo_node,
                        nome,
                        linha * DISTANCIA_NODES,
                        coluna * DISTANCIA_NODES);
  }
}
