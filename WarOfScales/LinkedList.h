#pragma once
#include "Utilities.h"
#include <stdio.h>
#include <string.h>
#define MEMBER_MAP(Y) \
Y(UIElement) \
Y(GameElement) \
Y(DiffusionField) \
Y(Line)

typedef struct Node
{
	int index, x, y, w, h;
	double val; //mostly for debugging
	struct Node *nextNode, *previousNode;
	//remember to update member map
	struct UIElement* UIElement;
	struct GameElement* GameElement;
	struct DiffusionField* DiffusionField;
	struct Line* Line;
	SDL_Texture* texture;
	char* string;
}Node;
typedef struct NodeList
{
	int listSize;
	Node *headNode;
	Node *lastNode;
}NodeList;

void NodeList_Init(NodeList* list);
NodeList* NodeList_Create();
void NodeList_AddNode(NodeList* list, Node* n);
void NodeList_PushNode(NodeList* list, Node* n);
void NodeList_InsertNode(NodeList* list, Node* n1, Node* node);
BOOLEAN NodeList_Contains(NodeList* list, Node* n);
void NodeList_RemoveNode(NodeList* list, Node* n, BOOLEAN destroy);
void NodeList_FindUIElement(Node** output, NodeList* list, struct UIElement* element);
void NodeList_Clear(NodeList* list);
Node* NodeList_GetNode(NodeList* list, int depth, BOOLEAN reverse);
void NodeList_ReverseListOrder(NodeList* list);
void NodeList_Print(NodeList* list);
void Node_Copy(Node* output, Node* input);
void Node_Destroy(Node* node);
void Node_Init(Node* node);
Node* Node_Create();
Node* Node_GetNode(Node* n, int depth, BOOLEAN reverse);