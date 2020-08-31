#include <stdio.h>
#include <stdlib.h>

//structures
/*CARD*/
typedef struct {
    char nombre;
    char type;
}CARD;
/*PILE*/
typedef struct Element{
    CARD* data;
    struct Element* next;
}Element;

//prototypes
/*CARD*/
CARD* createCard(char nombre, char type);
/*PILE*/
Element* createElement(CARD* crd);
int isEmpty(Element* PIL);
void push(CARD* crd, Element** PIL);
CARD* peek(Element* PIL);
void pop(Element** PIL);
void distroyPil(Element** PIL);

//implementations
/*CARD*/
CARD* createCard(char nombre, char type){
    CARD* crd = (CARD*) malloc(sizeof(CARD));
    if(crd){
        crd->nombre = nombre;
        crd->type = type;
    }
    return crd;
}
/*PILE*/
Element* createElement(CARD* crd){
    Element* elm = (Element*) malloc(sizeof(Element));
    elm->data = crd;
    elm->next = NULL;
    return elm;
}

int isEmpty(Element* PIL){
    return PIL==NULL ? 1 : 0;
}

void push(CARD* crd, Element** PIL){
    Element* elm = createElement(crd);
    elm->next = *PIL;
    *PIL = elm;
}

CARD* peek(Element* PIL){
    return isEmpty(PIL)==0? PIL->data : NULL;
}

void pop(Element** PIL){
    if(!isEmpty(*PIL)){
        Element* pPIL= *PIL;
        *PIL = (*PIL)->next;
        free(pPIL->data);
        free(pPIL);
    }
}

void distroyPil(Element** PIL){
    while(!isEmpty(*PIL)){
        pop(PIL);
    }
}

int main(void){
    Element* PIL = NULL;
    push(createCard(1,'g'),&PIL);
    printf("card number %d, card type %c\n",peek(PIL)->nombre, peek(PIL)->type);
    push(createCard(12,'s'),&PIL);
    printf("card number %d, card type %c\n",peek(PIL)->nombre, peek(PIL)->type);
    pop(&PIL);
    printf("card number %d, card type %c\n",peek(PIL)->nombre, peek(PIL)->type);
    distroyPil(&PIL);
    return 0;
}