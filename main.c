#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

//structures
/*CARD*/
typedef struct {
    int nombre;
    int type;
}CARD;
/*PILE*/
typedef struct Element{
    CARD* data;
    struct Element* next;
}Element;
/*GAME*/
typedef struct {
    CARD* freeCell[4];
    Element* fondationPIL[4];
    Element* columnsPIL[8];
}GAME;

//prototypes
/*CARD*/
CARD* createCard(int** saturation);

/*PILE*/
Element* createElement(CARD* crd);
int isEmpty(Element* PIL);
int push(CARD* crd, Element** PIL);
CARD* peek(Element* PIL);
void pop(Element** PIL);
void distroyPil(Element** PIL);

/*GAME*/
int gameInit(GAME** game);
int cardMove(CARD* crd, int destination);
int displayGame(GAME* game);
int distroyGame(GAME** game);

//implementations
/*CARD*/
CARD* createCard(int** saturation){
    int randomType;
    int randomNumber;
    CARD* crd = (CARD*) malloc(sizeof(CARD));
    if(crd){
        struct timeval tv;
        gettimeofday(&tv,NULL);
        randomType = tv.tv_usec % 4;
        gettimeofday(&tv,NULL);
        randomNumber = tv.tv_usec % 12;
        while(saturation[randomNumber][randomType]==4){
            gettimeofday(&tv,NULL);
            randomType = tv.tv_usec % 4;
            gettimeofday(&tv,NULL);
            randomNumber = tv.tv_usec % 12;
        }
        saturation[randomNumber][randomType]+=1;
        crd->nombre = randomNumber;
        crd->type = randomType;
        return crd;
    }
    return NULL;
}

/*PILE*/
Element* createElement(CARD* crd){
    if(!crd){
        return NULL;
    }
    Element* elm = (Element*) malloc(sizeof(Element));
    elm->data = crd;
    elm->next = NULL;
    return elm;
}

int isEmpty(Element* PIL){
    return PIL==NULL ? 1 : 0;
}

int push(CARD* crd, Element** PIL){
    Element* elm = createElement(crd);
    if(!elm){
        return 1;
    }
    elm->next = *PIL;
    *PIL = elm;
    return 0;
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
/*GAME*/
int gameInit(GAME** game){
    int** saturation = NULL;
    saturation = (int**) malloc(sizeof(int*)*12);
    (*game) = (GAME*) malloc(sizeof(GAME));
    if(*game){
        int i = 0, j = 0;

        //creating saturation dynamic table
        for(i=0; i < 12 ; i++){
            saturation[i] = (int*) malloc(sizeof(int)*4);
        }

        //initialisation of saturation table and stacks
        for(i=0 ; i<12 ; i++){
            for(j=0; j<4 ; j++){
                saturation[i][j] = 0;
            }
        }
        for(i=0 ; i<4 ; i++){
            ((*game)->freeCell)[i] = NULL;
        }
        for(i=0 ; i<4 ; i++){
            ((*game)->fondationPIL)[i] = NULL;
        }
        for(i=0 ; i<8 ; i++){
            ((*game)->columnsPIL)[i] = NULL;
        }

        //adding cards to columns
        for(i=0; i < 8 ; i++){
            int numberOfCardsByColumn = 7;
            if(i==4)
                numberOfCardsByColumn = 6;
            for(j=0 ; j < numberOfCardsByColumn ; j++){
                if(push(createCard(saturation),&(((*game)->columnsPIL)[i]))){
                    printf("ERROR: Couldnt push!!\n");
                    exit(EXIT_FAILURE);
                }
            }
        }

        //free saturation dynamic table
        for(i=0; i < 12 ; i++){
            free(saturation[i]);
        }
        free(saturation);
        return 0;
    }
    return 1;
}

int cardMove(CARD* crd, int destination){
    return 0;
}

int displayGame(GAME* game){
    int i = 0, j = 0;
    char type[8];
    CARD crd;
    for(i = 0 ; i < 8 ; i++){
        while(!isEmpty((game->columnsPIL)[i])){
            crd = *peek((game->columnsPIL)[i]);
            pop(&((game->columnsPIL)[i]));
            switch(crd.type){
                case 0 : strcpy(type, "heart");break;
                case 1 : strcpy(type, "club");break;
                case 2 : strcpy(type, "spade");break;
                case 3 : strcpy(type, "diamond");break;
                default : printf("ERROR: Invalid card type\n"); exit(EXIT_FAILURE); 
            }
            printf("[%d-%s]\t",crd.nombre, type);
        }
        printf("\n\n");
    }
    return 0;
}

int distroyGame(GAME** game){
    int i = 0;
    for(i=0; i<4; i++){
        free(((*game)->freeCell)[i]);
        distroyPil(&(((*game)->fondationPIL)[i]));
    }
    for(i=0; i<8; i++){
        distroyPil(&(((*game)->columnsPIL)[i]));
    }
    return 0;
}

int main(void){
    GAME* game;

    if(gameInit(&game)){
        printf("ERROR: Initialisation failure");
    }
    if(displayGame(game)){
        printf("ERROR: Display failure");
    }
    if(distroyGame(&game)){
        printf("ERROR: distroy failure");
    }

    return 0;
}