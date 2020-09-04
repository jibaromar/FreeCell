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
    Element* foundationPIL[4];
    Element* columnPIL[8];
}GAME;

//prototypes
/*CARD*/
CARD* createCard(int nombre, int type);
CARD* createRandomCard(int** saturation);
int moveCardBetweenColumn(int cardIndex, Element** sourcePIL, Element** destinationPIL);
int moveCardFromFreeCellToFoundation(CARD** freeCell, Element** foundationPIL);
int moveCardFromFreeCellToColumn(CARD** freeCell, Element** columnPIL);
int moveCardToFreeCell(Element** columnPIL, CARD** freeCell);
int moveCardFromColumnToFundation(Element** columnPIL, Element** foundationPIL);

/*PILE*/
Element* createElement(CARD* crd);
int isEmpty(Element* PIL);
int push(CARD* crd, Element** PIL);
CARD* peek(Element* PIL);
void pop(Element** PIL);
void distroyPil(Element** PIL);

/*GAME*/
int gameInit(GAME** game);
int displayGame(GAME* game);
int distroyGame(GAME** game);

//implementations
/*CARD*/
CARD* createCard(int nombre, int type){
    CARD* crd = (CARD*) malloc(sizeof(CARD));
    if(crd){
        crd->nombre = nombre;
        crd->type = type;
    }
    return crd;
}

CARD* createRandomCard(int** saturation){
    int randomType;
    int randomNumber;
    CARD* crd = (CARD*) malloc(sizeof(CARD));
    if(crd){
        struct timeval tv;
        gettimeofday(&tv,NULL);
        randomType = tv.tv_usec % 4;
        gettimeofday(&tv,NULL);
        randomNumber = tv.tv_usec % 13;
        while(saturation[randomNumber][randomType]!=0){
            gettimeofday(&tv,NULL);
            randomType = tv.tv_usec % 4;
            gettimeofday(&tv,NULL);
            randomNumber = tv.tv_usec % 13;
        }
        saturation[randomNumber][randomType]+=1;
        crd->nombre = randomNumber;
        crd->type = randomType;
        return crd;
    }
    return NULL;
}

int moveCardBetweenColumn(int cardIndex, Element** sourcePIL, Element** destinationPIL){
    if(!sourcePIL || !destinationPIL){
        printf("ERROR: invalide parameter\n");
        return 1;
    }
    
    Element* PIL = NULL;
    int i = 0;
    if(isEmpty(*destinationPIL)){
        for(i = 0; i<=cardIndex; i++){
            if(!isEmpty(*sourcePIL)){
                push(createCard(peek(*sourcePIL)->nombre, peek(*sourcePIL)->type),&PIL);
                pop(sourcePIL);
                continue;
            }
            printf("ERROR: empty columnPIL");
            return 1;
        }
        for(i = 0; i<=cardIndex; i++){
            push(createCard(peek(PIL)->nombre, peek(PIL)->type),destinationPIL);
            pop(&PIL);
        }
        return 0;
    }
    else{
        Element* pPIL = *sourcePIL;
        for(i = 0; i < cardIndex ;i++){
            pPIL = pPIL->next;
        }
        if(pPIL->data->nombre == (*destinationPIL)->data->nombre-1 && (pPIL->data->type + (*destinationPIL)->data->type)%2==1){
            for(i = 0; i<=cardIndex; i++){
                if(!isEmpty(*sourcePIL)){
                    push(createCard(peek(*sourcePIL)->nombre, peek(*sourcePIL)->type),&PIL);
                    pop(sourcePIL);
                    continue;
                }
                printf("ERROR: empty columnPIL");
                return 1;
            }
            for(i = 0; i<=cardIndex; i++){
                push(createCard(peek(PIL)->nombre, peek(PIL)->type),destinationPIL);
                pop(&PIL);
            }
            return 0;
        }
        printf("WARNING: Insatisfaction of conditions\n");
        return 1;
    }
    return 1;
}

int moveCardFromFreeCellToFoundation(CARD** freeCell, Element** foundationPIL){
    if(!freeCell || !foundationPIL){
        printf("ERROR: invalide parameter\n");
        return 1;
    }
    if(*freeCell){
        if(!isEmpty(*foundationPIL)){
            if((*freeCell)->nombre == (*foundationPIL)->data->nombre+1 && (*freeCell)->type == (*foundationPIL)->data->type){
                push(createCard((*freeCell)->nombre, (*freeCell)->type),foundationPIL);
                free((*freeCell));
                (*freeCell) = NULL;
                return 0;
            }
            printf("WARNING: Insatisfaction of conditions\n");
            return 1;
        }
        if((*freeCell)->nombre == 0){
            push(createCard((*freeCell)->nombre, (*freeCell)->type),foundationPIL);
            free((*freeCell));
            (*freeCell) = NULL;
            return 0;
        }
        printf("WARNING: Insatisfaction of conditions\n");
        return 1;
    }
    printf("WARNING: empty freeCell\n");
    return 1;
}

int moveCardFromFreeCellToColumn(CARD** freeCell, Element** columnPIL){
    if(!freeCell || !columnPIL){
        printf("ERROR: invalide parameter\n");
        return 1;
    }
    if((*freeCell)){
        if(!isEmpty(*columnPIL)){
            if((*freeCell)->nombre == (*columnPIL)->data->nombre-1 && ((*freeCell)->nombre + (*columnPIL)->data->nombre) % 2 == 1){
                push(createCard((*freeCell)->nombre, (*freeCell)->type),columnPIL);
                free((*freeCell));
                (*freeCell) = NULL;
                return 0;
            }
            printf("WARNING: Insatisfaction of conditions\n");
            return 1;
        }
        push(createCard((*freeCell)->nombre, (*freeCell)->type),columnPIL);
        free((*freeCell));
        (*freeCell) = NULL;
        return 0;
    }
    printf("WARNING: empty freeCell\n");
    return 1;
}

int moveCardToFreeCell(Element** columnPIL, CARD** freeCell){
    if(!freeCell || !columnPIL){
        printf("ERROR: invalide parameter\n");
        return 1;
    }
    if(!(*freeCell)){
        if(!isEmpty(*columnPIL)){
            CARD* crd  = peek(*columnPIL);
            (*freeCell) = createCard(crd->nombre, crd->type);
            pop(columnPIL);
            return 0;
        }
        printf("ERROR: empty stack\n");
        return 1;
    }
    printf("WARNING: non empty freeCell\n");
    return 1;
}

int moveCardFromColumnToFundation(Element** columnPIL, Element** foundationPIL){
    if(!columnPIL || !foundationPIL){
        printf("ERROR: invalide parameter\n");
        return 1;
    }
    if(isEmpty(*columnPIL)){
        printf("ERROR: empty stack\n");
        return 1;
    }
    CARD* crd = peek(*columnPIL);
    if(isEmpty(*foundationPIL)){
        if(crd->nombre == 0){
            push(createCard(crd->nombre, crd->type),foundationPIL);
            pop(columnPIL);
            return 0;
        }
        printf("WARNING: Insatisfaction of conditions\n");
        return 1;
    }
    if(crd->nombre == (*foundationPIL)->data->nombre+1 && crd->type == (*foundationPIL)->data->type){
        push(createCard(crd->nombre, crd->type),foundationPIL);
        pop(columnPIL);
        return 0;
    }
    printf("WARNING: Insatisfaction of conditions\n");
    return 1;
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
    if(!crd){
        printf("ERROR: crd null pointer\n");
        return 1;
    }
    Element* elm = createElement(crd);
    if(!elm){
        printf("ERROR: dynamic memory allocation \n");
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
    saturation = (int**) malloc(sizeof(int*)*13);
    (*game) = (GAME*) malloc(sizeof(GAME));
    if(*game){
        int i = 0, j = 0;

        //creating saturation dynamic table
        for(i=0; i < 13 ; i++){
            saturation[i] = (int*) malloc(sizeof(int)*4);
        }

        //initialisation of saturation table and stacks
        for(i=0 ; i<13 ; i++){
            for(j=0; j<4 ; j++){
                saturation[i][j] = 0;
            }
        }
        for(i=0 ; i<4 ; i++){
            ((*game)->freeCell)[i] = NULL;
        }
        for(i=0 ; i<4 ; i++){
            ((*game)->foundationPIL)[i] = NULL;
        }
        for(i=0 ; i<8 ; i++){
            ((*game)->columnPIL)[i] = NULL;
        }

        //adding cards to columns
        int numberOfCardsByColumn = 7;
        for(i=0; i < 8 ; i++){
            if(i==4)
                numberOfCardsByColumn = 6;
            for(j=0 ; j < numberOfCardsByColumn ; j++){
                if(push(createRandomCard(saturation),&(((*game)->columnPIL)[i]))){
                    printf("ERROR: Couldnt push!!\n");
                    exit(EXIT_FAILURE);
                }
            }
        }

        //free saturation dynamic table
        for(i=0; i < 13 ; i++){
            free(saturation[i]);
        }
        free(saturation);
        return 0;
    }
    return 1;
}


int displayGame(GAME* game){
    if(!game){
        printf("ERROR: Initialisation failure game null pointer\n");
        return 1;
    }
    int i = 0, j = 0;
    char type[8];
    CARD crd;
    Element** pPIL = NULL;

    printf("FREECELLS\n");
    for(i = 0 ; i < 4 ; i++){
        if(!(game->freeCell[i])){
            printf("(%d)[ - ]\t",i);
            continue;
        }
        crd = *(game->freeCell[i]);
        switch(crd.type){
                case 0 : strcpy(type, "heart");break;
                case 1 : strcpy(type, "spade");break;
                case 2 : strcpy(type, "diamond");break;
                case 3 : strcpy(type, "club");break;
                default : printf("ERROR: Invalid card type\n"); exit(EXIT_FAILURE); 
        }
        printf("(%d)[%d-%s]\t",i, crd.nombre, type);
    }
    
    printf("\nFOUNDATIONCELLS\n");
    for(i = 0 ; i < 4 ; i++){
        if(!(game->foundationPIL[i])){
            printf("(%d)[ - ]\t",i);
            continue;
        }
        crd = *(game->foundationPIL[i]->data);
        switch(crd.type){
                case 0 : strcpy(type, "heart");break;
                case 1 : strcpy(type, "spade");break;
                case 2 : strcpy(type, "diamond");break;
                case 3 : strcpy(type, "club");break;
                default : printf("ERROR: Invalid card type\n"); exit(EXIT_FAILURE); 
        }
        printf("(%d)[%d-%s]\t",i, crd.nombre, type);
    }

    printf("\n\nCOLUMNCELLS\n");
    for(i = 0 ; i < 8 ; i++){
        pPIL = &((game->columnPIL)[i]);
        printf("(%d) ",i);
        while(!isEmpty(*pPIL)){
            crd = *peek(*pPIL);
            pPIL = &((*pPIL)->next);
            switch(crd.type){
                case 0 : strcpy(type, "heart");break;
                case 1 : strcpy(type, "spade");break;
                case 2 : strcpy(type, "diamond");break;
                case 3 : strcpy(type, "club");break;
                default : printf("ERROR: Invalid card type\n"); exit(EXIT_FAILURE); 
            }
            printf("[%d-%s]\t",crd.nombre, type);
        }
        printf("\n\n");
    }
    
    printf("\n==================================================================\n");
    return 0;
}

int distroyGame(GAME** game){
    int i = 0;
    for(i=0; i<4; i++){
        free(((*game)->freeCell)[i]);
        distroyPil(&(((*game)->foundationPIL)[i]));
    }
    for(i=0; i<8; i++){
        distroyPil(&(((*game)->columnPIL)[i]));
    }
    return 0;
}

int main(void){
    GAME* game;
    char command[10];
    gameInit(&game);
    displayGame(game);

    while(1){
        scanf("%s", command);
        if(command[0] == 'C'){
            char number[3];
            int columnNumber = 0;
            number[0]=command[1];
            number[1]=command[2];
            number[2]='\0';
            columnNumber = atoi(number);
            if(command[3] == 'C'){
                int columnNumber2 = 0;
                number[0]=command[4];
                number[1]=command[5];
                number[2]='\0';
                columnNumber2 = atoi(number);
                
                moveCardBetweenColumn(0, &(game->columnPIL[columnNumber]), &(game->columnPIL[columnNumber2]));
                displayGame(game);
            }
            else if(command[3] == 'F'){
                if(command[4] == 'R'){
                    int freeCellNumber = 0;
                    number[0]=command[5];
                    number[1]=command[6];
                    number[2]='\0';
                    freeCellNumber = atoi(number);

                    moveCardToFreeCell(&(game->columnPIL[columnNumber]), &(game->freeCell[freeCellNumber]));
                    displayGame(game);
                }
                else if(command[4] == 'O'){
                    int foundationCellNumber = 0;
                    number[0]=command[5];
                    number[1]=command[6];
                    number[2]='\0';
                    foundationCellNumber = atoi(number);

                    moveCardFromColumnToFundation(&(game->columnPIL[columnNumber]), &(game->foundationPIL[foundationCellNumber]));
                    displayGame(game);
                }
            }
        }
        else if(command[0] == 'F'){
            if(command[1] == 'R'){
                char number[3];
                int freeCellNumber = 0;
                number[0]=command[2];
                number[1]=command[3];
                number[2]='\0';
                freeCellNumber = atoi(number);

                if(command[4] == 'C'){
                    int columnNumber = 0;
                    number[0]=command[5];
                    number[1]=command[6];
                    number[2]='\0';
                    columnNumber = atoi(number);

                    moveCardFromFreeCellToColumn(&(game->freeCell[freeCellNumber]), &(game->columnPIL[columnNumber]));
                    displayGame(game);
                }
                else if(command[4] == 'F'){
                    if(command[5] == 'O'){
                        int foundationCellNumber = 0;
                        number[0]=command[6];
                        number[1]=command[7];
                        number[2]='\0';
                        foundationCellNumber = atoi(number);

                        moveCardFromFreeCellToFoundation(&(game->freeCell[freeCellNumber]), &(game->foundationPIL[foundationCellNumber]));
                        displayGame(game);
                    }
                }
            }
        }
    }

    distroyGame(&game);

    return 0;
}