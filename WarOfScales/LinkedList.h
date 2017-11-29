#pragma once
#include "Utilities.h"
#include <stdio.h>
#include <string.h>
#define MEMBER_MAP(Y) \
Y(UIElement) \
Y(GameElements.GameElement) \
Y(GameElements.Unit) \
/*Y(DiffusionField) \*/ \
Y(Geom.Point) \
Y(Geom.Line) \
Y(texture)

enum NodeFlags {
	NFlag_Disabled = 1,
	NFlag_Visible = 2,
	NFlag_Blocking1 = 4,
	NFlag_Blocking3 = 8,
	NFlag_Moved = 16, //unit moved grid spaces
	NFlag_Stationary = 32, //unit is not moving
};
typedef struct Node
{
	int index, x, y, w, h, flags;
	double val; //mostly for debugging
	struct Node *nextNode, *previousNode, *Node;
	//remember to update member map
	struct UIElement* UIElement;
	union GameElements {
		struct GameElement* GameElement;
		struct Unit* Unit;
	}GameElements;
	//struct DiffusionField* DiffusionField;
	union Geom {
		struct Point* Point;
		struct Line* Line;
	}Geom;
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
void NodeList_Clear(NodeList* list, BOOLEAN destroy);
void NodeList_Destroy(NodeList* list);
Node* NodeList_GetNode(NodeList* list, int depth, BOOLEAN reverse);
void NodeList_GetNodesWithFlags(NodeList* out, NodeList* in, int withflags, BOOLEAN withall, int withoutflags, BOOLEAN withoutall);
void NodeList_ReverseListOrder(NodeList* list);
void NodeList_Print(NodeList* list);
void Node_Copy(Node* output, Node* input);
void Node_Destroy(Node* node);
void Node_Init(Node* node);
Node* Node_Create();
Node* Node_GetNode(Node* n, int depth, BOOLEAN reverse);
void Node_EnableFlags(Node* node, int flags);
void Node_DisableFlags(Node* node, int flags);
void Node_ToggleFlags(Node* node, int flags);
void Node_SetFlags(Node* node, int flags, BOOLEAN set);