#include "includes.h"

enum NODE_TYPE
{
  centroid = 0,
  entry_exit = 1,
  intersection = 2
};

enum LINK_TYPE
{
  freeway = 1,
  ramp = 2,
  urbanRoad = 3
};

enum LANE_RULES
{
  canGoStraightAhead = 0,
  canChangeRight = 1,
  canChangeLeft = 2,
  etc = 64,
  hov = 128
};

typedef struct lane
{
  int ID;
  int rule;
  struct lane *next;
} lane;
typedef struct lane *linkLane;

typedef struct laneConnector
{
  int ID1;
  int ID2;
  struct laneConnector *next;
} laneConnector;
typedef struct laneConnector *linkLaneConnector;

typedef struct segment
{
  int ID;
  int defaultSpeedLimit;
  int freeSpeed;
  int grad;
  int x0;
  int y0;
  int bulge;
  int x1;
  int y1;
  struct segment *next;
} segment;
typedef struct segment *linkSegment;

typedef struct upDnLink
{
  int linkID;
  struct upDnLink *next;
} upDnLink;
typedef struct upDnLink *linkUpDnLink;

typedef struct link
{
  int ID;
  int type;
  int upNodeID;
  /* ToDo: padronizar em dnNodeID */
  int dnNodeID;
  int labelID;
  segment *Segments;
  lane *Lanes;
  struct link *next;
} link;
typedef struct link *linkLink;

typedef struct node 
{
  int ID;
  int type;
  char *name;
  int x;
  int y;
  upDnLink *upLinks;
  upDnLink *dnLinks;
} node;
typedef struct node *linkNode;

typedef struct nodeID
{
  int nodeID;
  struct nodeID *next;
} nodeID;
typedef struct nodeID *linkNodeID;

/*
	newNode:
  cria um novo Node. recebe dados e ponteiro para um proximo.
  cria, preenche e devolve um link para o 
*/
linkNode newNode (int ID, int type, char *name, int x, int y) {
  linkNode nNode = malloc(sizeof(*nNode));

  printf("[newNode]: Criando Node %d\n", ID);
  nNode->ID = ID;
  nNode->type = type;
	nNode->name = NULL;
  nNode->x = x;
  nNode->y = y;
  nNode->upLinks = NULL;
  nNode->dnLinks = NULL;

	return nNode;
}

linkLane newLane (int ID, int rule, linkLane next) 
{
	linkLane nLane = malloc(sizeof(*nLane));

  printf("[newLane]: Criando Lane %d\n", ID);
  nLane->ID = ID;
  nLane->rule = rule;
  nLane->next = next;

  return nLane;
}

linkSegment newSegment (int ID, int defaultSpeedLimit, int freeSpeed, int grad, int x0, int y0, int bulge, int x1, int y1, linkSegment next)
{
  linkSegment nSegment = malloc(sizeof(*nSegment));

  printf("[newSegment]: Criando Segment %d\n", ID);
  nSegment->ID = ID;
  nSegment->defaultSpeedLimit = defaultSpeedLimit;
  nSegment->freeSpeed = freeSpeed;
  nSegment->grad = grad;
  nSegment->x0 =x0 ;
  nSegment->y0 = y0;
  nSegment->bulge = bulge;
  nSegment->x1 = x1;
  nSegment->y1 = y1;
  nSegment->next = next;
  
  return nSegment;
}
    
/*
	 ToDo: testar se o link existe
	 ToDo: adicionar dois links pro node recebido (fazer uma nova funcao que faz isso)
*/
linkLink newLink (int ID, int type, int upNodeID, int dnNodeID, int labelID, linkLink next)
{
  linkLink nLink = malloc (sizeof (*nLink));

  printf("[newLink]: Criando Link %d\n", ID);
  nLink->ID = ID;
  nLink->type = type;
  nLink->upNodeID = upNodeID;
  nLink->dnNodeID = dnNodeID;
  nLink->labelID = labelID;
  nLink->next = next;
  nLink->Segments = NULL;
  nLink->Lanes = NULL;

  return nLink;
}

linkUpDnLink newUpDnLink (int linkID, linkUpDnLink next)
{
  linkUpDnLink nUpDnLink = malloc (sizeof (*nUpDnLink));

  printf("[newUpDnLink]: Criando UpDnLink %d\n", linkID);
  nUpDnLink->linkID = linkID;
  nUpDnLink->next = next;

  return nUpDnLink;
}

linkLaneConnector newLaneConnector (int ID1, int ID2, linkLaneConnector next)
{
  linkLaneConnector nLaneConnector = malloc (sizeof (*nLaneConnector));

  printf("[newLaneConnector]: Criando Lane Connection %d-%d\n", ID1, ID2);
  nLaneConnector->ID1 = ID1;
  nLaneConnector->ID2 = ID2;
  nLaneConnector->next = next;

  return nLaneConnector;
}

linkNodeID newNodeID (int nodeID, linkNodeID next)
{
  linkNodeID nNodeID = malloc (sizeof (*nNodeID));

  printf("[newNodeID]: Criando NodeID %d\n", nodeID);
  nNodeID->nodeID = nodeID;
  nNodeID->next = next;

  return nNodeID;
}

linkLink procuraLink(linkLink Links, int linkID)
{
  linkLink p;

  printf("[procuraLink]: Localizando Link %d\n", linkID);
  for(p = Links; p != NULL; p = p->next)
    if(p->ID == linkID)
      return p;

  return NULL;
}
