#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NPROC 5
#define IO_total 3

// Estrutura para representar uma operação de I/O
typedef struct {
    int tipo;  // 0 - impressora, 1 - disco, 2 - fita
    int tempo;
} IO;

//struct para guardar as informações do processo
typedef struct {
    int PID;
    int tempo_esperado;
    //int prioridade; //talvez nao precisa
    int status;
    IO *listaIO;  // Lista de operações de I/O
    int num_io;         // Número de operações de I/O
} processo;

//guarda informações da fila
typedef struct {
    int itens[NPROC];
    int inicio;
    int fim;
} Fila ;


void inicializaFila(Fila *fila);
int filaVazia(Fila *fila);
int filaCheia(Fila *fila);
void insereFila(Fila *fila, int valor);
void removeFila(Fila *fila);
void printFila(Fila *fila);
int peek(Fila *fila);
void roundRobin(processo processos[], int quantum);

void roundRobin(processo processos[], int quantum){
    int tempo_total = 0, i = 0;
    int processos_concluidos = 0;

    Fila altaPrioridade;
    Fila baixaPrioridade;

    //inicializando filas
    inicializaFila(&altaPrioridade);
    inicializaFila(&baixaPrioridade);

    //inserindo processos novos na fila de alta prioridade
    for(i=0;  i<NPROC; i++){
        insereFila(&altaPrioridade, i);
    }

    printf("TEMPO DECORRIDO: 0\n");
    //se a fila de alta prioridade não estiver vazia
    while(processos_concluidos != NPROC){
        int p;
        if(!(filaVazia(&altaPrioridade))){
            p = peek(&altaPrioridade);
            //processo mais prioritario consumindo sua fatia de tempo
            processos[p].tempo_esperado = processos[p].tempo_esperado - quantum;
            removeFila(&altaPrioridade);

            //se nao precisa de mais processamento, fica como concluido
            if (processos[p].tempo_esperado <= 0){
                processos[p].status = 2; //mudando estado para concluido
                processos_concluidos++;
                printf("Processo %d terminou\n", processos[p].PID);

                if(processos[p].tempo_esperado < 0){
                    //ajustando contagem de tempo
                    tempo_total = tempo_total + processos[p].tempo_esperado;
                }
            }

            //se ainda tiver tempo, manda pra fila de baixa prioridade
            else{
                printf("Processo %d foi para baixa prioridade\n", processos[p].PID);
                printf("Tempo restante: %d\n", processos[p].tempo_esperado);
                insereFila(&baixaPrioridade, p);
            }

        }

        //se a fila de alta prioridade estiver vazia;
        else if(!(filaVazia(&baixaPrioridade))){
            p = peek(&baixaPrioridade);
            processos[p].tempo_esperado = processos[p].tempo_esperado - quantum;
            removeFila(&baixaPrioridade);

            //se o processo não tiver mais demanda
            if (processos[p].tempo_esperado <= 0){
                processos[p].status = 2; // atualiza estado para concluido
                processos_concluidos++;
                printf("Processo %d terminou\n", processos[p].PID);

                if(processos[p].tempo_esperado < 0){
                    //ajustando contagem de tempo
                    tempo_total = tempo_total + processos[p].tempo_esperado;
                }
            }
            else{
                printf("Processo %d sofreu preempção\n", processos[p].PID);
                printf("Tempo restante: %d\n", processos[p].tempo_esperado);
                insereFila(&baixaPrioridade, p);
            }

        }
        
        /*printf("Alta prioridade: ");
        printFila(&altaPrioridade);
        printf("Baixa prioridade: ");
        printFila(&baixaPrioridade);*/

        tempo_total += quantum;
        printf("\nTEMPO DECORRIDO: %d\n", tempo_total);
    }

    printf("\nTEMPO DECORRIDO TOTAL: %d\n", tempo_total);
}


/*-------------- Manutenção de Fila ------------------*/

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
        //printf("Elemento %d inserido na fila.\n", valor);
    }
}

void removeFila(Fila *fila) {
    if (filaVazia(fila)) {
        printf("Ops, fila vazia\n");
        return;
    } else {
        //int removedItem = fila->itens[fila->inicio];
        //printf("Elemento %d removido da fila\n", removedItem);
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

int peek(Fila *fila) {
    if (filaVazia(fila)) {
        printf("Ops, fila vazia. Não há primeiro item.\n");
        return -1;  // Ou algum valor que indique que a fila está vazia
    } else {
        return fila->itens[fila->inicio];
    }
}

int main(int argc, char *argv[]){
    int i;
    
    //tempo esperado de cada um dos processos
    int tempos[NPROC]; // = {4, 2, 8, 9, 6};
    
    // Lendo arquivo de entrada
    if (argc < 2){
        printf("Digite: %s <nome do arquivo de entrada>.txt\n", argv[0]);
        return 0;
    }

    char nomeArquivo[100];
    strcpy(nomeArquivo, argv[1]);
    FILE *arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL){
        printf("Erro ao abrir o arquivo. Encerrando o programa...\n");
        return 0;
    }
    // printf("Arquivo aberto com sucesso!\n");
    
    char virgula;
    for(int j = 0; j < NPROC; j++){
        if (fscanf(arquivo, "%d%c", &tempos[j], &virgula) == 2){
            // printf("%d\n", tempos[j]);
        }
    }

    int IO_list[IO_total][3]; //= {{0, 1, 3}, {1, 2, 1}, {2, 1, 5}}; // p.PID, tipo, tempo de chegada

    for (int a = 0; a < 3; a++){
        for(int j = 0; j < IO_total; j++){
            if(fscanf(arquivo, "%d%c", &IO_list[j][a], &virgula) == 2){
            // printf("%d\n", IO_list[j][a]);
            }
        }
    }

    fclose(arquivo);

    //criando lista de processos;
    processo processos[NPROC];

    //criando os processos e guardando no array de processos
    for(i=0; i<NPROC; i++){
        processo p;
        p.PID = i;
        p.tempo_esperado = tempos[i];
        p.status = 0; //pronto
        p.num_io = 0;
        p.listaIO = NULL;
        for(int j = 0; j<IO_total; j++){
            if(IO_list[j][0]==i){ // é IO desse processo
              p.num_io++; // inicializa o número de operações de I/O
              p.listaIO = realloc(p.listaIO, p.num_io * sizeof(IO));
              p.listaIO[p.num_io - 1].tipo = IO_list[j][1]; // Configura o tipo de I/O
              p.listaIO[p.num_io - 1].tempo = IO_list[j][2]; // Configura o tempo de chegada
            }
        }
        

        processos[i] = p;
    }
    
    roundRobin(processos, 2);

    return 0;
}
