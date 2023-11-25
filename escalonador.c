#include <stdio.h>
#include <stdlib.h>

#define NPROC 5

typedef struct {
    int PID;
    int tempo_esperado;
    //int prioridade; //talvez nao precisa
    int status; 
} processo;


typedef struct {
    int itens[NPROC];
    int inicio;
    int fim;
}Fila ;


void inicializaFila(Fila *fila);

int filaVazia(Fila *fila);

int filaCheia(Fila *fila);

void insereFila(Fila *fila, int valor);

void removeFila(Fila *fila);

void printFila(Fila *fila);


void main(){
    int i;

    //criando lista de processos;
    processo processos[NPROC];

    //tempo esperado de cada um dos processos
    int tempos[NPROC] = {10, 12, 22, 6, 34};

    //criando os processos
    for(i=0; i<NPROC; i++){
        processo p;
        p.PID = i;
        p.tempo_esperado = tempos[i];
        p.status = 0; //pronto
    }

    //roundRobin(processos, 2);
}



void roundRobin(processo processos[], int quantum){
    int tempo_total = 0, i = 0;
    int processos_concluidos = 0;

    Fila altaPrioridade;
    Fila baixaPrioridade;

    inicializaFila(&altaPrioridade);
    inicializaFila(&baixaPrioridade);

    for(i=0;  i<NPROC; i++){
        insereFila(&altaPrioridade, i);
    }

    while(processos_concluidos != NPROC){
        //olhar a lista de alta prioridade
        /*se a lista nao estiver vazia, pegar o processo do inicio da fila, 
        remover o quantum do seu tempo esperado e se não terminar passa ele pra fila
        baixa prioridade

        se a fila estiver vazia
        */


        //olhar a lista de baixa prioridade


    }

}

void inicializaFila(Fila *fila) {
    fila->inicio = -1;
    fila->fim = -1;
}

int filaVazia(Fila *fila) {
    return (fila->inicio == -1 && fila->fim == -1);
}

int filaCheia(Fila *fila) {
    return (fila->fim + 1) % NPROC == fila->inicio;
}

void insereFila(Fila *fila, int valor) {
    if (filaCheia(fila)) {
        printf("Ops, fila cheia\n");
        return;
    } else {
        if (filaVazia(fila)) {
            fila->inicio = 0;
        }
        fila->fim = (fila->fim + 1) % NPROC;
        fila->itens[fila->fim] = valor;
        printf("Elemento %d inserido na fila.\n", valor);
    }
}

void removeFila(Fila *fila) {
    if (filaVazia(fila)) {
        printf("Ops, fila vazia\n");
        return;
    } else {
        int removedItem = fila->itens[fila->inicio];
        printf("Elemento %d removido da fila\n", removedItem);
        if (fila->inicio == fila->fim) {
            // Se só tiver um elemento na fila
            inicializaFila(fila);
        } else {
            fila->inicio = (fila->inicio + 1) % NPROC;
        }
    }
}

void printFila(Fila *fila) {
    printf("Valores da fila: ");
    int i = fila->inicio;

    do {
        printf("%d ", fila->itens[i]);
        i = (i + 1) % NPROC;
    } while (i != (fila->fim + 1) % NPROC);

    printf("\n");
}
