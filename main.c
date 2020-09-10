#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <sys/time.h>
#include <string.h>

/*Graphics Macros*/
#define WINDOW_WIDTH 1300
#define WINDOW_HEIGHT 730

#define CARD_WIDTH 133
#define CARD_HEIGHT 204
#define CARD_HEAD 38

#define FREECELL_0_X 27
#define FREECELL_1_X 164
#define FREECELL_2_X 301
#define FREECELL_3_X 438
#define FOUNDATION_0_X 726
#define FOUNDATION_1_X 863
#define FOUNDATION_2_X 1000
#define FOUNDATION_3_X 1137
#define FREECELL_FOUNDATION_Y 20

#define COLUMN_0_X 55
#define COLUMN_1_X 206
#define COLUMN_2_X 358
#define COLUMN_3_X 509
#define COLUMN_4_X 661
#define COLUMN_5_X 812
#define COLUMN_6_X 964
#define COLUMN_7_X 1115
#define COLUMN_Y 253

#define START_BUTTON_X 474
#define START_BUTTON_Y 547
#define START_BUTTON_W 353
#define START_BUTTON_H 90

#define START_SCREEN_WALLPAPER "SDL/img/bitmap/freeCell-Graphics-start.bmp"
#define PLAY_SCREEN_WALLPAPER "SDL/img/bitmap/freeCell-Graphics-playground.bmp"
#define CARDS_WALLPAPER "SDL/img/bitmap/freeCell-Graphics-cards.bmp"
#define YOUWIN_WALLPAPER "SDL/img/bitmap/freeCell-Graphics-uwin.bmp"
#define YOULOSE_WALLPAPER "SDL/img/bitmap/freeCell-Graphics-ulose.bmp"


#define CLOSE_WINDOW_REQUEST 2
#define START_GAME_REQUEST 0
#define START_BUTTON_CLICKED 1

#define YOU_WIN 2
#define YOU_LOSE -1

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
    int numberOfCardsOnColumn[8];
    CARD* freeCell[4];
    Element* foundationPIL[4];
    Element* columnPIL[8];
    int columnsCoordinates[16];
    SDL_Renderer* renderer;
    SDL_Window* window;
}GAME;

//prototypes
/*CARD*/
CARD* createCard(int nombre, int type);
CARD* createRandomCard(int** saturation);
int moveCardBetweenColumn(GAME* game, int cardIndex, int sourcePIL, int destinationPIL);
int moveCardFromFreeCellToFoundation(CARD** freeCell, Element** foundationPIL);
int moveCardFromFreeCellToColumn(GAME* game, int freeCellIndex,int columnIndex);
int moveCardToFreeCell(GAME* game, int freeCellIndex,int columnIndex);
int moveCardFromColumnToFundation(GAME* game, int columnIndex, int foundationIndex);

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

/*Graphics*/
void ExitWithError(char* Error, SDL_Surface* surface, SDL_Texture* texture, SDL_Renderer* renderer, SDL_Window* window);
SDL_Texture* SurfaceToTexture(char* link, SDL_Renderer* renderer, SDL_Window* window);
int handelMouseClick();
int CheckCardPil(GAME* game, int x, int y, int* srcColumn, int* cardIndex);
int CheckSerie(GAME* game, int srcPIL, int cardIndex);
int DisplayWhileMoving(GAME* game, int srcColumn, int cardIndex, Element* PIL, SDL_Texture* bitmapTextureCards);
int CheckYouWin(GAME* game);
int CheckYouLose(GAME* game);

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

int moveCardBetweenColumn(GAME* game, int cardIndex, int sourcePIL, int destinationPIL){
    Element* PIL = NULL;
    int i = 0;
    if(isEmpty(game->columnPIL[destinationPIL])){
        for(i = 0; i<cardIndex; i++){
            if(!isEmpty(game->columnPIL[sourcePIL])){
                push(createCard(peek(game->columnPIL[sourcePIL])->nombre, peek(game->columnPIL[sourcePIL])->type),&PIL);
                pop(&(game->columnPIL[sourcePIL]));
                continue;
            }
            printf("ERROR: empty columnPIL");
            return 1;
        }
        for(i = 0; i<cardIndex; i++){
            push(createCard(peek(PIL)->nombre, peek(PIL)->type),&(game->columnPIL[destinationPIL]));
            pop(&PIL);
        }
        game->numberOfCardsOnColumn[sourcePIL] -= cardIndex;
        game->numberOfCardsOnColumn[destinationPIL] += cardIndex;
        return 0;
    }
    else{
        Element* pPIL = game->columnPIL[sourcePIL];
        printf("number of cards is %d\n", cardIndex);
        for(i = 0; i < cardIndex-1 ;i++){
            pPIL = pPIL->next;
        }
        printf("down=%d, up=%d\n",pPIL->data->nombre,(game->columnPIL[destinationPIL])->data->nombre+1);
        if(pPIL->data->nombre == (game->columnPIL[destinationPIL])->data->nombre+1 && (pPIL->data->type + (game->columnPIL[destinationPIL])->data->type)%2==1){
            for(i = 0; i<cardIndex; i++){
                if(!isEmpty(game->columnPIL[sourcePIL])){
                    push(createCard(peek(game->columnPIL[sourcePIL])->nombre, peek(game->columnPIL[sourcePIL])->type),&PIL);
                    pop(&(game->columnPIL[sourcePIL]));
                    continue;
                }
                printf("ERROR: empty columnPIL");
                return 1;
            }
            for(i = 0; i<cardIndex; i++){
                push(createCard(peek(PIL)->nombre, peek(PIL)->type),&(game->columnPIL[destinationPIL]));
                pop(&PIL);
            }
            game->numberOfCardsOnColumn[sourcePIL] -= cardIndex;
            game->numberOfCardsOnColumn[destinationPIL] += cardIndex;
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
            if((*freeCell)->nombre == (*foundationPIL)->data->nombre-1 && (*freeCell)->type == (*foundationPIL)->data->type){
                push(createCard((*freeCell)->nombre, (*freeCell)->type),foundationPIL);
                free((*freeCell));
                (*freeCell) = NULL;
                return 0;
            }
            printf("WARNING: Insatisfaction of conditions\n");
            return 1;
        }
        if((*freeCell)->nombre == 12){
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

int moveCardFromFreeCellToColumn(GAME* game, int freeCellIndex,int columnIndex){
    if(game->freeCell[freeCellIndex]){
        if(!isEmpty(game->columnPIL[columnIndex])){
            if((game->freeCell[freeCellIndex])->nombre == (game->columnPIL[columnIndex])->data->nombre-1 && ((game->freeCell[freeCellIndex])->nombre + (game->columnPIL[columnIndex])->data->nombre) % 2 == 1){
                push(createCard((game->freeCell[freeCellIndex])->nombre, (game->freeCell[freeCellIndex])->type),&(game->columnPIL[columnIndex]));
                free((game->freeCell[freeCellIndex]));
                (game->freeCell[freeCellIndex]) = NULL;
                game->numberOfCardsOnColumn[columnIndex]++;
                return 0;
            }
            printf("WARNING: Insatisfaction of conditions\n");
            return 1;
        }
        push(createCard((game->freeCell[freeCellIndex])->nombre, (game->freeCell[freeCellIndex])->type),&(game->columnPIL[columnIndex]));
        free((game->freeCell[freeCellIndex]));
        (game->freeCell[freeCellIndex]) = NULL;
        game->numberOfCardsOnColumn[columnIndex]++;
        return 0;
    }
    printf("WARNING: empty freeCell\n");
    return 1;
}

int moveCardToFreeCell(GAME* game, int freeCellIndex,int columnIndex){
    if(!(game->freeCell[freeCellIndex])){
        if(!isEmpty(game->columnPIL[columnIndex])){
            CARD* crd  = peek(game->columnPIL[columnIndex]);
            (game->freeCell[freeCellIndex]) = createCard(crd->nombre, crd->type);
            pop(&(game->columnPIL[columnIndex]));
            return 0;
        }
        printf("ERROR: empty stack\n");
        return 1;
    }
    printf("WARNING: non empty freeCell\n");
    return 1;
}

int moveCardFromColumnToFundation(GAME* game, int columnIndex, int foundationIndex){
    if(isEmpty(game->columnPIL[columnIndex])){
        printf("ERROR: empty stack\n");
        return 1;
    }
    CARD* crd = peek(game->columnPIL[columnIndex]);
    if(isEmpty(game->foundationPIL[foundationIndex])){
        if(crd->nombre == 12){
            push(createCard(crd->nombre, crd->type),&(game->foundationPIL[foundationIndex]));
            pop(&(game->columnPIL[columnIndex]));
            game->numberOfCardsOnColumn[columnIndex]--;
            return 0;
        }
        printf("WARNING: Insatisfaction of conditions\n");
        return 1;
    }
    if(crd->nombre == (game->foundationPIL[foundationIndex])->data->nombre-1 && crd->type == (game->foundationPIL[foundationIndex])->data->type){
        push(createCard(crd->nombre, crd->type),&(game->foundationPIL[foundationIndex]));
        pop(&(game->columnPIL[columnIndex]));
        game->numberOfCardsOnColumn[columnIndex]--;
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
    Element* elm = createElement(createCard(crd->nombre, crd->type));
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

        /* Game Kernel */
        for(i=0 ; i < 16 ; i++){
            (*game)->numberOfCardsOnColumn[i] = 0;
        }

        (*game)->renderer = NULL;
        (*game)->window = NULL;

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
                (*game)->numberOfCardsOnColumn[i]++;
            }
        }

        //free saturation dynamic table
        for(i=0; i < 13 ; i++){
            free(saturation[i]);
        }
        free(saturation);

        /* Game Graphics */
        //coordinatesTable initialisation
        (*game)->columnsCoordinates[0] = FREECELL_0_X;
        (*game)->columnsCoordinates[1] = FREECELL_1_X;
        (*game)->columnsCoordinates[2] = FREECELL_2_X;
        (*game)->columnsCoordinates[3] = FREECELL_3_X;
        (*game)->columnsCoordinates[4] = FOUNDATION_0_X; 
        (*game)->columnsCoordinates[5] = FOUNDATION_1_X; 
        (*game)->columnsCoordinates[6] = FOUNDATION_2_X; 
        (*game)->columnsCoordinates[7] = FOUNDATION_3_X;
        (*game)->columnsCoordinates[8] = COLUMN_0_X;
        (*game)->columnsCoordinates[9] = COLUMN_1_X;
        (*game)->columnsCoordinates[10] = COLUMN_2_X;
        (*game)->columnsCoordinates[11] = COLUMN_3_X;
        (*game)->columnsCoordinates[12] = COLUMN_4_X;
        (*game)->columnsCoordinates[13] = COLUMN_5_X;
        (*game)->columnsCoordinates[14] = COLUMN_6_X;
        (*game)->columnsCoordinates[15] = COLUMN_7_X;


        //SDL Graphics initialisation
        if(SDL_Init(SDL_INIT_VIDEO) != 0){
            ExitWithError("Failed to initialise SDL", NULL, NULL, NULL, NULL);
        }

        (*game)->window = SDL_CreateWindow("FreeCell", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

        if(!((*game)->window)){
            ExitWithError("Failed to create window", NULL, NULL, NULL, NULL);
        }

        (*game)->renderer = SDL_CreateRenderer((*game)->window, -1, SDL_RENDERER_SOFTWARE);

        if(!((*game)->renderer)){
            ExitWithError("Failed to create renderer", NULL, NULL, NULL, (*game)->window);
        }

        //Printing start screen
        SDL_Texture* bitmapTexture = NULL;
        bitmapTexture = SurfaceToTexture(START_SCREEN_WALLPAPER, (*game)->renderer, (*game)->window);
        SDL_RenderCopy((*game)->renderer, bitmapTexture, NULL, NULL);
        SDL_RenderPresent((*game)->renderer);

        SDL_bool GameInitIsRunning = 1;

        while(GameInitIsRunning){
            SDL_Event event;
            while(SDL_PollEvent(&event)){
                switch (event.type)
                {
                case SDL_WINDOWEVENT:
                    if(event.window.event == SDL_WINDOWEVENT_CLOSE){
                        SDL_DestroyTexture(bitmapTexture);
                        return CLOSE_WINDOW_REQUEST;
                    }
                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == SDL_BUTTON_LEFT){
                        switch(handelMouseClick()){
                            case START_BUTTON_CLICKED : 
                                SDL_SetRenderDrawColor((*game)->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                                SDL_Rect rect;
                                rect.x = START_BUTTON_X - 5;
                                rect.y = START_BUTTON_Y - 5;
                                rect.h = START_BUTTON_H + 10;
                                rect.w = START_BUTTON_W + 10;
                                SDL_RenderFillRect((*game)->renderer, &rect);
                                rect.x = START_BUTTON_X;
                                rect.y = START_BUTTON_Y;
                                rect.h = START_BUTTON_H;
                                rect.w = START_BUTTON_W;
                                SDL_RenderCopy((*game)->renderer, bitmapTexture, &rect, &rect);
                                SDL_RenderPresent((*game)->renderer);
                                
                                GameInitIsRunning = 0;
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }

        SDL_DestroyTexture(bitmapTexture);
        return START_GAME_REQUEST;
    }
    return EXIT_FAILURE;
}

int displayGame(GAME* game){
    SDL_RenderClear(game->renderer);
    int i = 0, j = 0;
    SDL_Texture* bitmapTexture = NULL;
    SDL_Rect srcRect, destRect;
    srcRect.h = CARD_HEIGHT;
    destRect.h = CARD_HEIGHT;
    srcRect.w = CARD_WIDTH;
    destRect.w = CARD_WIDTH;
    bitmapTexture = SurfaceToTexture(PLAY_SCREEN_WALLPAPER, game->renderer, game->window);
    SDL_RenderCopy(game->renderer, bitmapTexture, NULL, NULL);
    bitmapTexture = SurfaceToTexture(CARDS_WALLPAPER, game->renderer, game->window);

    destRect.y = FREECELL_FOUNDATION_Y;
    for(i=0; i<4 ; i++){
        if(game->freeCell[i] != NULL){
            
            destRect.x = game->columnsCoordinates[i];
            srcRect.x = game->freeCell[i]->nombre * CARD_WIDTH;
            srcRect.y = game->freeCell[i]->type * CARD_HEIGHT;
            SDL_RenderCopy(game->renderer, bitmapTexture, &srcRect, &destRect);
        }
    }

    for(i=0; i<4 ; i++){
        if((game->foundationPIL)[i] != NULL){
            
            destRect.x = game->columnsCoordinates[i+4];
            srcRect.x = game->foundationPIL[i]->data->nombre * CARD_WIDTH;
            srcRect.y = game->foundationPIL[i]->data->type * CARD_HEIGHT;
            SDL_RenderCopy(game->renderer, bitmapTexture, &srcRect, &destRect);
        }
    }
    Element* pPIL = NULL, *PIL_TO_DISPLAY=NULL;
    for(i=0; i<8 ; i++){
        pPIL = game->columnPIL[i];
        while(pPIL != NULL){
            push(pPIL->data,&PIL_TO_DISPLAY);
            pPIL = pPIL->next;
        }
        destRect.x = game->columnsCoordinates[i+8];
        destRect.y = COLUMN_Y;
        while(PIL_TO_DISPLAY != NULL){
            srcRect.x = PIL_TO_DISPLAY->data->nombre * CARD_WIDTH;
            srcRect.y = PIL_TO_DISPLAY->data->type * CARD_HEIGHT;
            SDL_RenderCopy(game->renderer, bitmapTexture, &srcRect, &destRect);
            destRect.y += CARD_HEAD;
            pop(&PIL_TO_DISPLAY);
        }
    }
    SDL_DestroyTexture(bitmapTexture);
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
    SDL_DestroyRenderer((*game)->renderer);
    SDL_DestroyWindow((*game)->window);
    SDL_Quit();
    return 0;
}

int HandelMouseMovements(GAME* game){
    int x = 0, y = 0, srcColumn = -1, cardIndex = -1;
    SDL_GetMouseState(&x, &y);
    Element* PIL = NULL;
    int destColumn = -1;
    CheckCardPil(game, x, y, &srcColumn, &cardIndex);
    int result = CheckSerie(game, srcColumn, cardIndex);
    printf("column=%d, index=%d, possible=%d\n",srcColumn,cardIndex, result);
    if(result==0){
        SDL_Texture* bitmapCardsTexture = SurfaceToTexture(CARDS_WALLPAPER, game->renderer, game->window);
        switch(srcColumn){
            case 0: case 1: case 2: case 3:
                push(game->freeCell[srcColumn],&PIL);
                game->freeCell[srcColumn] = NULL;
                destColumn = DisplayWhileMoving(game, srcColumn, cardIndex, PIL, bitmapCardsTexture);
                game->freeCell[srcColumn] = createCard(peek(PIL)->nombre,peek(PIL)->type) ;pop(&PIL);
                displayGame(game);
                SDL_RenderPresent(game->renderer);
                switch(destColumn){
                    case 4: case 5: case 6: case 7: 
                        if(moveCardFromFreeCellToFoundation(&(game->freeCell[srcColumn]),&(game->foundationPIL[destColumn-4])) == 0){
                            displayGame(game);
                            SDL_RenderPresent(game->renderer);
                        }
                        break;
                    default:
                        if(moveCardFromFreeCellToColumn(game, srcColumn ,destColumn-8) ==0 ){
                            displayGame(game);
                            SDL_RenderPresent(game->renderer);
                        }
                }
                break;
            default:
                for(int i = 0; i < game->numberOfCardsOnColumn[srcColumn-8] - cardIndex ; i++){
                    push(peek(game->columnPIL[srcColumn-8]),&PIL);
                    pop(&(game->columnPIL[srcColumn-8]));
                }
                destColumn = DisplayWhileMoving(game, srcColumn, cardIndex, PIL, bitmapCardsTexture);
                for(int i = 0; i < game->numberOfCardsOnColumn[srcColumn-8] - cardIndex ; i++){
                    push(peek(PIL),&(game->columnPIL[srcColumn-8]));
                    pop(&(PIL));
                }
                displayGame(game);
                SDL_RenderPresent(game->renderer);
                switch(destColumn){
                    case 0: case 1: case 2: case 3: 
                        if(moveCardToFreeCell(game, destColumn, srcColumn-8) == 0){
                            displayGame(game);
                            SDL_RenderPresent(game->renderer);
                        }
                        break;
                    case 4: case 5: case 6: case 7: 
                        if(moveCardFromColumnToFundation(game, srcColumn-8, destColumn-4) == 0){
                            displayGame(game);
                            SDL_RenderPresent(game->renderer);
                        }
                        break;
                    default:
                        if(moveCardBetweenColumn(game, game->numberOfCardsOnColumn[srcColumn-8] - cardIndex, srcColumn-8 ,destColumn-8) == 0){
                            displayGame(game);
                            SDL_RenderPresent(game->renderer);
                        }
                }
        }
    }
    if(CheckYouWin(game) == 1){
        return YOU_WIN;
    }
    
    if(CheckYouLose(game) == 1){
        return YOU_LOSE;
    }
    
    return 0;
}

int CheckCardPil(GAME* game, int x, int y, int* srcColumn, int* cardIndex){
    *srcColumn = -1;
    *cardIndex = -1;
        
    if(y >= FREECELL_FOUNDATION_Y && y <= FREECELL_FOUNDATION_Y + CARD_HEIGHT){
        for(int i = 0 ; i < 8 ; i++){
            if(x >= game->columnsCoordinates[i] && x <= game->columnsCoordinates[i] + CARD_WIDTH){
                *srcColumn = i;
                break;
            }
        }
        return *srcColumn;
    }
    for(int i = 8 ; i < 16 ; i++){
        if(x >= game->columnsCoordinates[i] && x <= game->columnsCoordinates[i] + CARD_WIDTH){
            *srcColumn = i;
            for(int j = 0; j < game->numberOfCardsOnColumn[i-8] - 1; j++){
                if(y >= COLUMN_Y + CARD_HEAD * j && y <= COLUMN_Y + CARD_HEAD * (j+1)){
                    *cardIndex = j;
                    break;
                }
            }
            if(y >= COLUMN_Y + CARD_HEAD * game->numberOfCardsOnColumn[i-8]-1 && y <= COLUMN_Y + CARD_HEAD * game->numberOfCardsOnColumn[i-8]-1 + CARD_HEIGHT){
                *cardIndex = game->numberOfCardsOnColumn[i-8]-1;
            }
            break;
        }
    }
    return *srcColumn;
}
int CheckSerie(GAME* game, int srcPIL, int cardIndex){
    int i = 0;
    if(srcPIL == -1 ){
        return 1;
    }
    if(cardIndex == -1){
        if(srcPIL < 4)
            if(game->freeCell[srcPIL]!=NULL)
                return 0;
        if(srcPIL< 8)
            return 1;
    }
    srcPIL = srcPIL-8;
    Element* PIL = game->columnPIL[srcPIL];
    for(i=game->numberOfCardsOnColumn[srcPIL]-1; i>cardIndex; i--){
        // printf("game->numberOfCardsOnColumn[srcPIL]=%d\n",game->numberOfCardsOnColumn[srcPIL]);
        // printf("%d|%d\n",(PIL->data->nombre == (PIL->next->data->nombre - 1)), ((PIL->data->type + PIL->next->data->type) % 2 == 1));
        if((PIL->data->nombre == (PIL->next->data->nombre + 1)) && ((PIL->data->type + PIL->next->data->type) % 2 == 1)){
            PIL = PIL->next;
            continue;
        }
        return 1;
    }   
    return 0;
}

int DisplayWhileMoving(GAME* game, int srcColumn, int cardIndex, Element* PIL, SDL_Texture* bitmapTextureCards){
    int oldX = 0, oldY = 0, newX = 0, newY = 0;
    SDL_GetMouseState(&oldX, &oldY);
    SDL_bool ButtonClicked = 1;
    SDL_Rect srcRect;
    SDL_Rect destRect;
    srcRect.w = CARD_WIDTH;
    srcRect.h = CARD_HEIGHT;
    destRect.w = CARD_WIDTH;
    destRect.h = CARD_HEIGHT;
    int startX = game->columnsCoordinates[srcColumn], startY = srcColumn<8?FREECELL_FOUNDATION_Y:COLUMN_Y + CARD_HEAD * cardIndex;
    SDL_Event event;
    Element* pPIL = NULL;
    while(ButtonClicked){
        SDL_PollEvent(&event);
        if(event.type == SDL_MOUSEBUTTONUP){
            if(event.button.button == SDL_BUTTON_LEFT){
                ButtonClicked = 0;
            }
        }
        displayGame(game);
        srcRect.x = 0;
        srcRect.y = 0;
        destRect.x = startX;
        destRect.y = startY;
        SDL_GetMouseState(&newX, &newY);
        destRect.x += newX - oldX;
        destRect.y += newY - oldY;
        startX = destRect.x;
        startY = destRect.y;
        oldX = newX;
        oldY = newY;
        pPIL = PIL;
        while(pPIL!=NULL){
            srcRect.x = pPIL->data->nombre * CARD_WIDTH;
            srcRect.y = pPIL->data->type * CARD_HEIGHT;
            SDL_RenderCopy(game->renderer, bitmapTextureCards, &srcRect, &destRect);
            destRect.y += CARD_HEAD;
            pPIL = pPIL->next;
        }
        SDL_RenderPresent(game->renderer);
    }
    SDL_DestroyTexture(bitmapTextureCards);
    return CheckCardPil(game, newX, newY, &srcColumn, &cardIndex);
}
/*Graphics*/
void ExitWithError(char* Error, SDL_Surface* surface, SDL_Texture* texture, SDL_Renderer* renderer, SDL_Window* window){

    if(surface){
        SDL_FreeSurface(surface);
    }
    if(texture){
        SDL_DestroyTexture(texture);
    }
    if(renderer){
        SDL_DestroyRenderer(renderer);
    }
    if(window){
        SDL_DestroyWindow(window);
    }
    printf("ERROR: %s\n",Error);
    SDL_Quit();
    exit(EXIT_FAILURE);
}

SDL_Texture* SurfaceToTexture(char* link, SDL_Renderer* renderer, SDL_Window* window){
    SDL_Surface* bitmapSurface = SDL_LoadBMP(link);
    if(bitmapSurface == NULL){
        ExitWithError("Failed to load bitmap image", NULL, NULL, renderer, window);

    }
    SDL_Texture* bitmapTexture = SDL_CreateTextureFromSurface(renderer, bitmapSurface);
    if(bitmapTexture == NULL){
        ExitWithError("Failed to create texture from bitmap surface", bitmapSurface, NULL, renderer, window);

    }
    
    SDL_FreeSurface(bitmapSurface);
    
    return bitmapTexture;
}

int handelMouseClick(){
    int x = 0, y = 0;
    SDL_GetMouseState(&x, &y);
    if(y >= START_BUTTON_Y && y <= START_BUTTON_Y + START_BUTTON_H){
        if(x >= START_BUTTON_X && x <= START_BUTTON_X + START_BUTTON_W)
            return START_BUTTON_CLICKED;
    }
    return 0;
}

int CheckYouWin(GAME* game){
    for(int i = 0; i < 4 ; i++){
        if(game->foundationPIL[i]){
            if(game->foundationPIL[i]->data->nombre != 0){
                return 0;
            }
            continue;
        }
        return 0;
    }
    return 1;
}

int CheckYouLose(GAME* game){
    if(game->freeCell[0] == NULL || game->freeCell[1] == NULL ||game->freeCell[2] == NULL ||game->freeCell[3] == NULL){
        printf("free cells\n");
        return 0;
    }
    for(int i = 0 ; i < 4 ; i++){
        for(int j = 0; j< 4 ; j++){
            if(game->freeCell[i]->nombre == game->foundationPIL[j]->data->nombre -1 && game->freeCell[i]->type == game->foundationPIL[j]->data->type){
                return 0;
            }
        }
        
        for(int j = 0; j< 8 ; j++){
            if(game->freeCell[i]->nombre == game->columnPIL[j]->data->nombre +1 && (game->freeCell[i]->type + game->columnPIL[j]->data->type) % 2 == 1){
                return 0;
            }
        }
    }
    for(int i = 0 ; i < 8 ; i++){
        for(int j = 0; j< 4 ; j++){
            if(game->columnPIL[i]->data->nombre == game->foundationPIL[j]->data->nombre -1 && game->columnPIL[i]->data->type == game->foundationPIL[j]->data->type){
                return 0;
            }
        }
        
        for(int j = i+1; j< 8 ; j++){
            Element* pPIL = game->columnPIL[i];
            while(1){
                
                if(pPIL->data->nombre == game->columnPIL[j]->data->nombre +1 && (pPIL->data->type + game->columnPIL[j]->data->type)%2==1){
                    return 0;
                }
                if(pPIL->data->nombre == pPIL->next->data->nombre +1 && (pPIL->data->type + pPIL->next->data->type) % 2 == 1){
                    pPIL = pPIL->next;
                    continue;
                }
                break;
            }
        }
    }
    return 1;
}

int main(void){
    GAME* game;
    SDL_bool GameIsRunning = 1;
    switch(gameInit(&game)){
        case START_GAME_REQUEST :
            displayGame(game);
            SDL_RenderPresent(game->renderer);
            break;
        case CLOSE_WINDOW_REQUEST :
            GameIsRunning = 0;
            break;
        default :
            distroyGame(&game);
            return EXIT_FAILURE;
    }
    while(GameIsRunning){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch (event.type)
            {
            case SDL_WINDOWEVENT:
                if(event.window.event == SDL_WINDOWEVENT_CLOSE){
                    GameIsRunning = 0;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(event.button.button == SDL_BUTTON_LEFT){
                    SDL_Texture* bitmapBackGround = NULL;
                    switch(HandelMouseMovements(game)){
                        case YOU_WIN: 
                            bitmapBackGround = SurfaceToTexture(YOUWIN_WALLPAPER, game->renderer, game->window);
                            SDL_RenderClear(game->renderer);
                            SDL_RenderCopy(game->renderer,bitmapBackGround, NULL, NULL);
                            SDL_RenderPresent(game->renderer);
                            SDL_DestroyTexture(bitmapBackGround);
                            break;
                        case YOU_LOSE: printf("you lose\n");break;
                            bitmapBackGround = SurfaceToTexture(YOULOSE_WALLPAPER, game->renderer, game->window);
                            SDL_RenderClear(game->renderer);
                            SDL_RenderCopy(game->renderer,bitmapBackGround, NULL, NULL);
                            SDL_RenderPresent(game->renderer);
                            SDL_DestroyTexture(bitmapBackGround);
                        default:
                        break;
                    }
                }
                break;
            default:
                break;
            }
        }
    }
    distroyGame(&game);

    return 0;
}