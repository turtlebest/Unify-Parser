/*
Your program will construct a type environment, and then perform unifications in that type environment, until
either the program ends, or a unification fails.

TYPE ::= TYPEVAR | PRIMITIVE_TYPE | FUNCTYPE | LISTTYPE;
PRIMITIVE_TYPE ::= ‘int’ | ‘float’ | ‘long’ | ‘string’;
TYPEVAR ::= ‘`’ VARNAME;  // Note, the character is a backwards apostrophe!
VARNAME ::= [a-zA-Z][a-zA-Z0-9]*; // Initial letter, then can have numbers
FUNCTYPE ::= ‘(‘ ARGLIST ‘)’ -> TYPE | ‘(‘ ‘)’ -> TYPE;
ARGLIST ::=   TYPE ‘,’ ARGLIST | TYPE;
LISTTYPE ::= ‘[‘ TYPE ‘]’;
 */

#include <string.h>
#include <stdlib.h>
#include<stdio.h>

#define TRUE 1
#define FALSE 0

#define DEFAULT_TYPE 0
#define FUN_TYPE 1
#define LIST_TYPE 2
#define VAR_TYPE 3
#define INT_TYPE 4
#define REAL_TYPE 5
#define STR_TYPE 6

struct node {
    char* value;
    int datatype;
    int Checked;
    struct node *firstchild;
    struct node *nextsibling;
};

struct Symbol {
    char *symbol[2];
    struct node* PointerToNode;
    int PointerDataType;
};

int FunctionType(char** Input, struct node *UnifyTree);
int PrimitiveType(char** Input, struct node *UnifyTree);
int TypeVar(char** Input, struct node *UnifyTree);
int VarName(char** Input, struct node *UnifyTree);
int ArgList(char** Input, struct node *UnifyTree);
int ListType(char** Input, struct node *UnifyTree);

struct Symbol SymbolTable[20];

int TableNumber = 0;

struct node* temp;

void RemoveSpaces(char* source)
{
  char* i = source;
  char* j = source;
  while(*j != 0)
  {
    *i = *j++;
    if(*i != ' ')
      i++;
  }
  *i = 0;
}

char Next(char** Source)
{ 
  char ch;

  ch = **Source;
  *Source = (*Source + 1);

  return ch;
}

char Peek(char** Source)
{
  char ch;

  ch = **Source;
  return ch;
}

struct node *CreateNode()
{
    struct node *NewNode = malloc(sizeof(struct node));
    
    NewNode->value = (char *)malloc(20*sizeof(char));
    memset(NewNode->value, 0, sizeof(20));
    NewNode->datatype = DEFAULT_TYPE;
    NewNode->firstchild = NULL;
    NewNode->nextsibling = NULL;
    NewNode->Checked = FALSE;
    
    return NewNode;
}

void GoThroughT (struct node *UnifyTree)
{
    struct node *Node = malloc(sizeof(struct node));
    struct node *temp = malloc(sizeof(struct node));

    printf("Tree:%s\n", UnifyTree->value);
    printf("%d\n", UnifyTree->datatype);
    Node = UnifyTree;

    while(Node->firstchild != NULL){
         printf("jj\n");
        printf("%p\n", Node->nextsibling);
        Node = Node->firstchild;
        temp = Node;
        while(Node->nextsibling != NULL){
            Node = Node->nextsibling;
            printf("Tree:%s\n", Node->value);
            printf("%d\n", Node->datatype);

        }
        Node = temp;
        printf("Tree:%s\n", Node->value);
        printf("%d\n", Node->datatype);
    }
}

int FindValinSymbolTable(char* value) {
    int i = 0;

    for (i = 0; i < TableNumber; i++) {
        if (!strcmp(SymbolTable[i].symbol[0], value) ||!strcmp(SymbolTable[i].symbol[1], value)){
            return TRUE;
        }
        
    }
    
    return FALSE;

}

struct node *FindDefinSymbolTable(struct node *Tree, int* TableNumberForSymbol) {
    int i = 0;

    for (i = 0; i < TableNumber; i++) {
        if (!strcmp(SymbolTable[i].symbol[0], Tree->value) ||!strcmp(SymbolTable[i].symbol[1], Tree->value)){
            if (SymbolTable[i].PointerToNode != NULL) {
              if (TableNumberForSymbol != NULL) {
                    *TableNumberForSymbol = i;
              }
                return SymbolTable[i].PointerToNode;
            }
            break;
        }
    }
    
    if (TableNumberForSymbol != NULL) {
        *TableNumberForSymbol = i;
    }
    
    return NULL;
}

void UpdateSymbolTable(char* UpdateSymbol, struct node *TreeToRecord) {
    int i = 0;
    
    for (i = 0; i < TableNumber; i++) {
        if(!strcmp(SymbolTable[i].symbol[0], UpdateSymbol)) {
            SymbolTable[i].PointerToNode = TreeToRecord;
            return;
        }
        
        if(!strcmp(SymbolTable[i].symbol[1], UpdateSymbol)) {
            if (SymbolTable[i].PointerToNode == NULL) {
                SymbolTable[i].PointerToNode = TreeToRecord;
                return;
            }
        }
    }
}

void UpdateSymbolTableCombine(char* UpdateSymbol, char* UpdateSymbol2) {
    int i = 0;
    
    for (i = 0; i < TableNumber; i++) {
        if(!strcmp(SymbolTable[i].symbol[0], UpdateSymbol)) {
            SymbolTable[i].symbol[1] = UpdateSymbol2;
        }
        
        if(!strcmp(SymbolTable[i].symbol[0], UpdateSymbol2)) {
            SymbolTable[i].symbol[0] = "";
        }
        
    }
}

int TypeCheck(struct node *Tree1, struct node *Tree2) {
    struct node *DefNode = CreateNode();
    struct node *DefNode2 = CreateNode();
    int Check = TRUE;
    
    if ((Tree1->datatype == FUN_TYPE && Tree2->datatype == FUN_TYPE) || (Tree1->datatype == LIST_TYPE && Tree2->datatype == LIST_TYPE)){
        Tree1 = Tree1->firstchild;
        Tree2 = Tree2->firstchild;
        while (Tree1->nextsibling != NULL && Tree2->nextsibling != NULL) {
            Check = TypeCheck(Tree1, Tree2);
            Tree1 = Tree1->nextsibling;
            Tree2 = Tree2->nextsibling;
            
            if (!Check) {
                return Check;
            }
        }
        
        Check = TypeCheck(Tree1, Tree2);
        return Check;
    }
    
    if ((Tree1->datatype == VAR_TYPE && Tree2->datatype == VAR_TYPE)){
        DefNode = FindDefinSymbolTable(Tree1, NULL);
        DefNode2 = FindDefinSymbolTable(Tree2, NULL);
        if (DefNode != NULL && DefNode2 != NULL ) {
          return TypeCheck(DefNode, DefNode2);
        }
        
        if (DefNode != NULL && DefNode2 == NULL) {
            UpdateSymbolTableCombine(Tree1->value, Tree2->value);
            return TRUE;
        }
        
        if (DefNode == NULL && DefNode2 != NULL) {
            UpdateSymbolTableCombine(Tree2->value, Tree1->value);
            return TRUE;
        }
        
        if (DefNode2 == NULL && DefNode == NULL) {
            UpdateSymbolTableCombine(Tree1->value, Tree2->value);
            return TRUE;
        }
    }
    
    if ((Tree1->datatype == VAR_TYPE && Tree2->datatype == FUN_TYPE)){
        DefNode = FindDefinSymbolTable(Tree1, NULL);
        if (DefNode != NULL) {
            return TypeCheck(DefNode, Tree2);
        } else {
            Tree1->datatype = Tree2->datatype;
            UpdateSymbolTable(Tree1->value, Tree2);
            return TRUE;
        }
    }
 
    if ((Tree1->datatype == FUN_TYPE && Tree2->datatype == VAR_TYPE)){
        DefNode = FindDefinSymbolTable(Tree2, NULL);
        if (DefNode != NULL) {
           return TypeCheck(Tree1, DefNode);
        } else {
            Tree2->datatype = Tree1->datatype;
            UpdateSymbolTable(Tree2->value, Tree1);
            return TRUE;
        }
    }
    
    if ((Tree1->datatype == VAR_TYPE && Tree2->datatype == LIST_TYPE)){
        DefNode = FindDefinSymbolTable(Tree1, NULL);
        if (DefNode != NULL) {
            return TypeCheck(DefNode, Tree2);
        } else {
            Tree1->datatype = Tree2->datatype;
            UpdateSymbolTable(Tree1->value, Tree2);
            return TRUE;
        }
    }
    
    if ((Tree1->datatype == LIST_TYPE && Tree2->datatype == VAR_TYPE)){
        DefNode = FindDefinSymbolTable(Tree2, NULL);
        if (DefNode != NULL) {
            return TypeCheck(Tree1, DefNode);
        } else {
            Tree2->datatype = Tree1->datatype;
            UpdateSymbolTable(Tree2->value, Tree1);
            return TRUE;
        }
    }
                      
    if (Tree1->datatype == VAR_TYPE){
        if (Tree2->datatype == INT_TYPE || Tree2->datatype == REAL_TYPE || Tree2->datatype == STR_TYPE) {
          DefNode = FindDefinSymbolTable(Tree1, NULL);
          if (DefNode != NULL) {
             return TypeCheck(DefNode, Tree2);
          } else {
             Tree1->datatype = Tree2->datatype;
             UpdateSymbolTable(Tree1->value, Tree2);
             return TRUE;
          }
        }
    }
        
        
    if (Tree2->datatype == VAR_TYPE){
        if (Tree1->datatype == INT_TYPE || Tree1->datatype == REAL_TYPE || Tree1->datatype == STR_TYPE) {
            DefNode = FindDefinSymbolTable(Tree2, NULL);
            if (DefNode != NULL) {
                return TypeCheck(Tree1, DefNode);
            } else {
                Tree2->datatype = Tree1->datatype;
                UpdateSymbolTable(Tree2->value, Tree1);
                return TRUE;
            }
        }
    }
    
    if ((Tree1->datatype == INT_TYPE && Tree2->datatype == INT_TYPE)|| (Tree1->datatype == REAL_TYPE && Tree2->datatype == REAL_TYPE)|| (Tree1->datatype == STR_TYPE && Tree2->datatype == STR_TYPE)){
        return TRUE;
    }
    
    return FALSE;
    
}

void PrintNode (struct node *Tree) {
    int i = 0;
    int functionPrintItem = 0;
    
    struct node *DefNode = CreateNode();


    if (Tree->datatype == INT_TYPE) {
        printf("int");
        return;
    }
    
    if (Tree->datatype == REAL_TYPE) {
        printf("real");
        return;
    }
    
    if (Tree->datatype == STR_TYPE) {
        printf("str");
        return;
    }
    
    
    if (Tree->datatype == LIST_TYPE) {
        if (Tree->value[0] != '['){
            Tree = FindDefinSymbolTable(Tree, NULL);
        }
        
        printf("[");
        Tree = Tree->firstchild;
        PrintNode(Tree);
        printf("]");
        return;
    }
    
    if (Tree->datatype == LIST_TYPE) {
        if (Tree->value[0] != '['){
            Tree = FindDefinSymbolTable(Tree, NULL);
        }
        
        printf("[");
        Tree = Tree->firstchild;
        PrintNode(Tree);
        printf("]");
        return;
    }

    if (Tree->datatype == FUN_TYPE) {
        if (Tree->value[0] != '('){
            Tree = FindDefinSymbolTable(Tree, NULL);
            //error control?
        }
        printf("(");
        Tree = Tree->firstchild;
        while (Tree->nextsibling != NULL) {
            PrintNode(Tree);
            Tree = Tree->nextsibling;
            
            functionPrintItem++;
            if (functionPrintItem == 1 && Tree->nextsibling != NULL) {
                printf(",");
            } else {
                printf(")->");
            }
        }

        if (functionPrintItem == 0) {
          printf(")->");
        }

        PrintNode(Tree);
        return;
    }

    if (Tree->datatype == VAR_TYPE) {
        int GetTableNum = 0;
        DefNode = FindDefinSymbolTable(Tree, &GetTableNum);

        if (DefNode != NULL) {
            Tree = DefNode;
            PrintNode(Tree);
        } else {
            printf("%s", SymbolTable[GetTableNum].symbol[0]);
        }
        return;
    }
}

int ParseType (char** Input,struct node *UnifyTree)
{
  char peeked;

  peeked = Peek(Input);

  switch (peeked) {
    case '(':
    	return FunctionType(Input, UnifyTree);
    break; 

    case '[':
        return ListType(Input, UnifyTree);
    break;  

    case '`':
        return TypeVar(Input, UnifyTree);
    break;  

    default:
        return PrimitiveType(Input, UnifyTree);
    break;    
  }

  return TRUE;
}

int PrimitiveType(char** Input, struct node *UnifyTree)
{
  char ch;
  char peeked;
  int i = 0;
  char *s;
  char *temp;
    
  s = (char *)malloc(20*sizeof(char));
  memset(s, 0, sizeof(20));
  
  peeked = Peek(Input);
  temp = *Input;

  switch (peeked) {
	case 'i':
         if  (temp[1] == 'n' && temp[2] == 't') {
            s = "int";
            for (i = 0; i < 3; i++) {
              Next(Input);
            }
            UnifyTree->value = s;
            UnifyTree->datatype = INT_TYPE;
         } else {
            return FALSE;
         }
         break;

    case 'r':
    	if  (temp[1] == 'e' && temp[2] == 'a' && temp[3] == 'l') {
    		s = "real";
            for (i = 0; i < 4; i++) {
              Next(Input);
            }
            UnifyTree->value = s;
            UnifyTree->datatype = REAL_TYPE;
         } else {
            return FALSE;
         }
         break;   

    case 's':
    	if  (temp[1] == 't' && temp[2] == 'r') {
    		s = "int";
    	    for (i = 0; i < 3; i++) {
              Next(Input);
            }
            UnifyTree->value = s;
            UnifyTree->datatype = STR_TYPE;
         } else {
            return FALSE;
         }
         break; 
    default:
    break;       
   }
    
   peeked = Peek(Input);
   if (peeked == ']'|| peeked == ','|| peeked == ')'|| peeked == '\0') {
   	 return TRUE;
   } else {
   	return FALSE;
   }
  
}



int FunctionType(char** Input, struct node *UnifyTree)
{
  char lparen;
  char rparen;
  char next;
  int NoErr = TRUE;
  int NoArg = FALSE;

  lparen = Next(Input);
  if (lparen != '(') {
    return FALSE;
  }
  UnifyTree->value = "()";
  UnifyTree->firstchild = CreateNode();
    
  rparen = Peek(Input);
  if (rparen != ')') {
	     NoErr = ArgList(Input, UnifyTree->firstchild);
         if (NoErr == FALSE) {
          return NoErr;
         }
         rparen = Next(Input);
         if (rparen != ')') {
          return FALSE;
        }
   } else {
     NoArg = TRUE;
     Next(Input);
   }

   UnifyTree->value = "()";
   UnifyTree->datatype = FUN_TYPE;
    
   next = Next(Input);
   if (next != '-')  {
        return FALSE;
   }
   
   next = Next(Input);
   if (next != '>')  {
        return FALSE;
   }

   struct node *Node;
   if (UnifyTree->firstchild->nextsibling != NULL) {
     UnifyTree->firstchild->nextsibling->nextsibling = CreateNode();
     Node =UnifyTree->firstchild->nextsibling->nextsibling;
   } else if (NoArg) {
     UnifyTree->firstchild = CreateNode();
     Node =UnifyTree->firstchild;
   } else {
      UnifyTree->firstchild->nextsibling = CreateNode();
      Node =UnifyTree->firstchild->nextsibling;
   }
    
   NoErr = ParseType(Input, Node);

   if (NoErr != TRUE) {
	 return NoErr;
   }

   return TRUE;
}

int ListType(char** Input, struct node *UnifyTree)
{
  char lparen;
  char rparen;
  char next;
  int NoErr = TRUE;

  lparen = Next(Input);

  if (lparen != '[') {
    return FALSE;
  }
  
  UnifyTree->value = "[";
  UnifyTree->firstchild = CreateNode();

  NoErr = ParseType(Input, UnifyTree->firstchild);
  if (NoErr != TRUE) {
    return NoErr;
  }

  rparen = Next(Input);
  if (rparen != ']') {
      return FALSE;
  }

  UnifyTree->value = "[]";
  UnifyTree->datatype = LIST_TYPE;

  return TRUE;
}

int TypeVar(char** Input, struct node *UnifyTree)
{	
  char lparen;
  int NoErr = TRUE;

  lparen = Next(Input);

  if (lparen != '`') {
	return FALSE;
  }

  NoErr = VarName(Input, UnifyTree);
  if (NoErr != TRUE) {
	return NoErr;
  }

  return TRUE;
}

int VarName(char** Input, struct node *UnifyTree)
{
  char next;
  char peeked;
  char *s;
  int err = TRUE;
  int NoneVar = FALSE;
  char cToStr[2];
    
  cToStr[1] = '\0';

  s = (char *)malloc(20*sizeof(char));
  memset(s, 0, sizeof(20));
  s[0] = '`';

 
  while(!NoneVar){	
  	next = Peek(Input);
    switch(next) {
	  case 'a'...'z':
	  case 'A'...'Z':
	  case '0'...'9':
        next = Next(Input);
            cToStr[0] = next;
	    s = strcat(s, cToStr);
	  break;  
	  default:
	    NoneVar = TRUE;
	}
  }
  
  
  UnifyTree->datatype = VAR_TYPE;
  UnifyTree->value = s;
    
  if (!FindValinSymbolTable(s)) {
    SymbolTable[TableNumber].symbol[0] = s;
    TableNumber++;
  }

  peeked = Peek(Input);

  if (peeked == ']'|| peeked == ','|| peeked == ')'|| peeked == '\0') {
  	 return TRUE;
  } else {
     return FALSE;
  }
}

int ArgList(char** Input, struct node *UnifyTree)
{
  char lparen;
  char rparen;
  char next;
  char peeked;
  int NoErr = TRUE;

  NoErr = ParseType(Input, UnifyTree);
  if (NoErr != TRUE) {
    return NoErr;
  }
	 
  peeked = Peek(Input);

  if (peeked == ',') {
    Next(Input);

    UnifyTree->nextsibling = CreateNode();
    NoErr = ArgList(Input, UnifyTree->nextsibling);

    if (NoErr != TRUE) {
      return NoErr;
    }
  }

  return TRUE;
}

int main()
{
   int m = 0;
   int n = 0;
   struct node *UnifyTreeArr[50];
   int UnifyTreeNum = 0;
   
    for (m = 0; m<20; m++){
       for (n = 0; n<2; n++) {
          SymbolTable[m].symbol[n] = (char *)malloc(100*sizeof(char));
       }
       SymbolTable[m].PointerToNode = NULL;
       SymbolTable[m].PointerDataType = DEFAULT_TYPE;
   }
    
   while(1) {
     char str[200], c;
     char *InputArray[20];
     int i =0;
     int k =0;
     int NoErr = TRUE;
     int NoErr2 = TRUE;
     struct node *UnifyTree1, *UnifyTree2;
       
     UnifyTreeArr[++UnifyTreeNum] = CreateNode();
     UnifyTree1 = UnifyTreeArr[UnifyTreeNum];
     UnifyTreeArr[++UnifyTreeNum] = CreateNode();
     UnifyTree2 = UnifyTreeArr[UnifyTreeNum];

     i = strlen(str);
     do
     { 
      scanf("%c", &c);
      str[i++] = c;
     } while (c != '\n');
     str[i - 1] = 0;       
     
     i = 0;

     for (k = 0; k<20; k++){
       InputArray[k] = (char *)malloc(100*sizeof(char));
     }

     RemoveSpaces(str);
       
     if (!strcmp(str, "QUIT")) {
         return FALSE;
     }

     InputArray[i] = strtok (str,"^");

     while (InputArray[i] != NULL){
       InputArray[++i] = strtok (NULL, "^");
     }
       
     if (i > 2 || i == 1) {
        printf("ERR\n");
        return FALSE;
     }

     NoErr = ParseType(&InputArray[0], UnifyTree1);
     NoErr2 = ParseType(&InputArray[1], UnifyTree2);

     if (!NoErr || !NoErr2) {
     	printf("ERR\n");
     	return FALSE;
     }

       if(!TypeCheck(UnifyTree1, UnifyTree2)){
           printf("BOTTOM\n");
           return FALSE;
       }

       PrintNode(UnifyTree1);
       printf("\n");
       
     memset(str, 0, sizeof(str));

   }

}
