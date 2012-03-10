/* ToDo: parametrizar quantidade de lanes */
#include "grafo.h"
#define MAX_NOME 1000
#define LARGURA_PIXEL 1000

/* arquivo de OD */
#define START_TIME 0
#define VEHICLE_TYPE 0
#define SCALING_FACTOR 1

typedef enum
{
  ESTRELA = 0,
  MALHA = 1
} estrategiasConexaoOD;

typedef enum
{
  IDA = 1,
  VOLTA = 2
} estrategiasSentidoOD;

int main () 
{
  int largura;
  int V;
  int nodeID, linkID, segmentID, laneID;
  int i;
  Graph G;
  linkNode * Nodes;
  linkLink Links;
  linkLaneConnector LaneConnectors;
  int qtdLinks, qtdSegments, qtdLanes, qtdLaneConnectors;
  Vertex v, w;
  FILE *arqNetwork;
  FILE *arqOD;
  char nomeArqNetwork[MAX_NOME];
  char nomeArqOD[MAX_NOME];
  linkLink p;
  linkSegment q;
  linkLane r;
  linkLaneConnector s;
  linkNodeID NodesBordaSuperior, nodSup;
  linkNodeID NodesBordaInferior, nodInf;
  linkNodeID NodesBordaEsquerda, nodEsq;
  linkNodeID NodesBordaDireita, nodDir;

  /* variaveis para conexao de lanes */
  /* ToDo: mudar nome pra linkUpDnLinkID */
  linkUpDnLink listaUpLinkID;
  linkUpDnLink listaDnLinkID;

  linkLink upLinkAtual, dnLinkAtual;
  linkLane listaLanesUp, listaLanesDn;

  int departureRate;

  int estrategiaConexaoOD;
  int estrategiaSentidoOD;

  nodeID = linkID = segmentID = laneID = qtdLaneConnectors = 0;
  
  LaneConnectors = NULL;
  Links = NULL;
  NodesBordaSuperior = NULL;
  NodesBordaInferior = NULL;
  NodesBordaEsquerda = NULL;
  NodesBordaDireita = NULL;

  /* inicializa grafo */
  printf("[malha]: Number of intersections: ");
  scanf("%d", &largura);
  V = largura * largura;

  printf("[malha]: Flow connection strategy (0 - star; 1 - grid): ");
  scanf("%d", &estrategiaConexaoOD);

  printf("[malha]: Flow direction (1 - unidirectional ->; 2 - unidirectional <-; 3 - bidirectional): ");
  scanf("%d", &estrategiaSentidoOD);

  printf("[malha]: Car flow (Cars/Hour): ");
  scanf("%d", &departureRate);

  G = GRAPHinit(V + 1);

  Nodes = malloc (V * sizeof(Nodes));

  /* conecta o grafo em malha */
    /* preenche dados do Node com base na topologia: Type, Name */
  printf("[GRAPHGridConnect]: conectando grafo");
  GRAPHGridConnect(G, largura, Nodes, &NodesBordaSuperior, &NodesBordaInferior, &NodesBordaEsquerda, &NodesBordaDireita);

  qtdLinks = qtdSegments = qtdLanes = 0;
  /* percorre o grafo e cria os links */
    /* segments */
      /* lanes */
  printf("[malha]: criando estruturas do MITSIMLab\n");
  for (v = 0; v < G->V; v++) {
    for (w = 0; w < G->V; w++) {
      if (G->adj[v][w]) { /* tem arco */
        /* cria link */
        /* newSegment (int ID, int defaultSpeedLimit, int freeSpeed, int grad, int x0, int y0, int bulge, int x1, int y1, linkSegment next) */
        Links = newLink (linkID,
                         urbanRoad,
                         v,
                         w,
                         0,
                         Links);

        /* atualiza lista de links para os nodes presentes no link */
        Nodes[v]->dnLinks = newUpDnLink(linkID, Nodes[v]->dnLinks);
        Nodes[w]->upLinks = newUpDnLink(linkID, Nodes[w]->upLinks);

        /* cria segment */
        /* newSegment (int ID, int defaultSpeedLimit, int freeSpeed, int grad, int x0, int y0, int bulge, int x1, int y1, linkSegment next) */
        /* ToDo: corrigir alocacao de coordenadas */
        Links->Segments = newSegment (segmentID,
                                      25,
                                      30,
                                      0,
                                      ((v - 1) / largura) * LARGURA_PIXEL, /* x0 */
                                      ((v - 1) % largura) * LARGURA_PIXEL, /* y0 */
                                      0,
                                      ((w - 1) / largura) * LARGURA_PIXEL, /* x0 */
                                      ((w - 1) % largura) * LARGURA_PIXEL, /* y0 */
                                      Links->Segments);

        /* cria lanes */
        /* newLane (int laneID, int rule, linkLane next) */
        Links->Lanes = newLane (laneID,
                                canGoStraightAhead + canChangeRight + canChangeLeft,
                                Links->Lanes);

        linkID++;
        laneID++;
        segmentID++;
      }
    }
  }

  /* acrescenta os dados de Node ao arquivo */
  printf("[malha]: criando arquivo network.dat\n");
  strcpy(nomeArqNetwork, "network.dat");
  arqNetwork = fopen(nomeArqNetwork, "w");

  fprintf(arqNetwork, "[Title] : \"network.dat\"\n");
  fprintf(arqNetwork, "\n");

  fprintf(arqNetwork, "[Link Labels] : 0\n{\n}\n");
  fprintf(arqNetwork, "\n");

  fprintf(arqNetwork, "[Nodes] : %d\n", V);
  fprintf(arqNetwork, "{\n");
  for(i = 1; i <= V; i++)
    fprintf(arqNetwork, "  {%d %d \"\"}\n", Nodes[i]->ID, Nodes[i]->type);
  fprintf(arqNetwork, "}\n");
  fprintf(arqNetwork, "\n");

  /* ToDo: resolver a quantificacao usada abaixo */
  /* Links */
  fprintf(arqNetwork, "[Links] : %d : %d : %d\n", linkID, segmentID, laneID);
  fprintf(arqNetwork, "{\n"); /* Links 01 */
  for(p = Links; p != NULL; p = p->next)
  {
    fprintf(arqNetwork, "  {%d %d %d %d %d\n", p->ID, p->type, p->upNodeID, p->dnNodeID, p->labelID); /* Links 02 */
    /* Segments */
    for(q = p->Segments; q!= NULL; q = q->next)
    {
      fprintf(arqNetwork, "    {%d %d %d %d\n", q->ID, q->defaultSpeedLimit, q->freeSpeed, q->grad); /* Segments 01 */
      fprintf(arqNetwork, "      {%d %d %d %d %d}\n", q->x0, q->y0, q->bulge, q->x1, q->y1); 

      /* Lanes */
      fprintf(arqNetwork, "       ");
      for(r = p->Lanes; r!= NULL; r = r->next)
        fprintf(arqNetwork, " {%d %d}", r->ID, r->rule); 
      fprintf(arqNetwork, "\n"); 

      fprintf(arqNetwork, "    }\n"); /* Segments 01 */
    }
    fprintf(arqNetwork, "  }\n"); /* Links 02 */
  }
  fprintf(arqNetwork, "}\n"); /* Links 01 */

  /* conecta as lanes */
  /*
  para cada node
    para cada upLink
      para cada lane (upLane)
        para cada dnLink
          para cada lane (dnLane)
            faz a conexao (nos dois sentidos?)
  */

  printf("[malha]: conectando lanes");
  qtdLaneConnectors = 0;
  for(i = 1; i <= V; i++)
  {
    /* Percorre os upLinks do Node */
    for(listaUpLinkID = Nodes[i]->upLinks; listaUpLinkID != NULL; listaUpLinkID = listaUpLinkID->next)
    {
      /* ToDo: fazer a funcao que procura o Link pelo ID obtido do Node atual */
      upLinkAtual = procuraLink(Links, listaUpLinkID->linkID);

      /* Percorre as lanes do upLink */
      for(listaLanesUp = upLinkAtual->Lanes; listaLanesUp != NULL; listaLanesUp = listaLanesUp->next)
      {
        /* Percorre os dnLinks do Node */
        for(listaDnLinkID = Nodes[i]->dnLinks; listaDnLinkID != NULL; listaDnLinkID = listaDnLinkID->next)
        {
          /* procura o Link pelo ID obtido do Node atual */
          dnLinkAtual = procuraLink(Links, listaDnLinkID->linkID);

          /* Percorre as lanes do dnLink */
          for(listaLanesDn = dnLinkAtual->Lanes; listaLanesDn != NULL; listaLanesDn = listaLanesDn->next)
          {
            /* ToDo: conecta lanes */
            /* testa se os links nao pertencem ao mesmo par de nodes */
            if(upLinkAtual->upNodeID != dnLinkAtual->dnNodeID)
            {
              LaneConnectors = newLaneConnector(listaLanesUp->ID, listaLanesDn->ID, LaneConnectors);
              qtdLaneConnectors++;
            }
          }
        }
      }
    }
  }
  printf("\n");

  fprintf(arqNetwork, "[Lane Connectors] : %d\n", qtdLaneConnectors);
  fprintf(arqNetwork, "{\n");
  for(s = LaneConnectors; s != NULL; s = s->next)
    fprintf(arqNetwork, "  {%d %d}\n", s->ID1, s->ID2);
  fprintf(arqNetwork, "}\n");

  /* cria arquivo de OD */
  /* ToDo: cabecalho do arquivo, resolver */
  
  printf("[malha]: criando arquivo demand.dat\n");
  strcpy(nomeArqOD, "demand.dat");
  arqOD = fopen(nomeArqOD, "w");

  fprintf(arqOD, "%d %d %d\n", START_TIME, VEHICLE_TYPE, SCALING_FACTOR);
  fprintf(arqOD, "{\n");

  printf("[malha]: estabelecendo origens/destinos");
  if(estrategiaConexaoOD == ESTRELA)
  {
    for(nodSup = NodesBordaSuperior; nodSup != NULL; nodSup = nodSup->next)
    {
      for(nodInf = NodesBordaInferior; nodInf != NULL; nodInf = nodInf->next)
      {
        /* cria linha de OD */
        /* ToDo: definir se sera nos dois sentidos */
        if(estrategiaSentidoOD == IDA || estrategiaSentidoOD == IDA + VOLTA)
          fprintf(arqOD, "  { %d %d %d }\n", nodSup->nodeID, nodInf->nodeID, departureRate);

        if(estrategiaSentidoOD == VOLTA || estrategiaSentidoOD == IDA + VOLTA)
          fprintf(arqOD, "  { %d %d %d }\n", nodInf->nodeID, nodSup->nodeID, departureRate);
      }
    }

    for(nodEsq = NodesBordaEsquerda; nodEsq != NULL; nodEsq = nodEsq->next)
    {
      for(nodDir = NodesBordaDireita; nodDir != NULL; nodDir = nodDir->next)
      {
        /* cria linha de OD */
        /* ToDo: definir se sera nos dois sentidos */

        /*
        StartTime | VehicleType | ScalingFactor
        {                             |>>>>>>>>>>>>>>>>>>>>>>>>>>>> essa parte nao e obrigatoria
          { Ori  |Des  |DepartureRate |Variance |Distribution |Path
            Node |Node |(Vehicle/Hr)  |(V/Hr)   |Factor (0-1) |ID   }
        }
        */

        if(estrategiaSentidoOD == IDA || estrategiaSentidoOD == IDA + VOLTA)
          fprintf(arqOD, "  { %d %d %d }\n", nodEsq->nodeID, nodDir->nodeID, departureRate);

        if(estrategiaSentidoOD == VOLTA || estrategiaSentidoOD == IDA + VOLTA)
          fprintf(arqOD, "  { %d %d %d }\n", nodDir->nodeID, nodEsq->nodeID, departureRate);
      }
    }
  } /* estrategia estrela */

  if(estrategiaConexaoOD == MALHA)
  {
    for(nodSup = NodesBordaSuperior, nodInf = NodesBordaInferior; nodSup != NULL && nodInf != NULL; nodSup = nodSup->next, nodInf = nodInf->next)
    {
      if(estrategiaSentidoOD == IDA || estrategiaSentidoOD == IDA + VOLTA)
        fprintf(arqOD, "  { %d %d %d }\n", nodSup->nodeID, nodInf->nodeID, departureRate);

      if(estrategiaSentidoOD == VOLTA || estrategiaSentidoOD == IDA + VOLTA)
        fprintf(arqOD, "  { %d %d %d }\n", nodInf->nodeID, nodSup->nodeID, departureRate);
    }

    for(nodEsq = NodesBordaEsquerda, nodDir = NodesBordaDireita; nodEsq != NULL && nodDir != NULL; nodEsq = nodEsq->next, nodDir = nodDir->next)
    {
      if(estrategiaSentidoOD == IDA || estrategiaSentidoOD == IDA + VOLTA)
        fprintf(arqOD, "  { %d %d %d }\n", nodEsq->nodeID, nodDir->nodeID, departureRate);

      if(estrategiaSentidoOD == VOLTA || estrategiaSentidoOD == IDA + VOLTA)
        fprintf(arqOD, "  { %d %d %d }\n", nodDir->nodeID, nodEsq->nodeID, departureRate);
    }
  } /* estrategia malha */

  fprintf(arqOD, "}\n");
  fprintf(arqOD, "<END>");

  printf("\n");

  fclose(arqOD);
  fclose(arqNetwork);

  printf("[malha]: malha criada com sucesso\n");

  return 0;
}
