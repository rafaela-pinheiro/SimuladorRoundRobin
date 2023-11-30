/*versao do escalonador que considera o tempo de chegada 
com apenas 1 fila de processos*/

#include <stdio.h>
#include <stdlib.h>
#define NPROC 5

//struct para guardar as informações do processo
typedef struct {
    int PID;
    int tempo_chegada;
    int tempo_esperado;
    int status; //0 para pronto, 1 para terminado
} processo;

typedef struct{
    int tipo;
    int t_chegada;
} IO;

//guarda informações da fila
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
int peek(Fila *fila);
void roundRobin(processo processos[], int quantum);

int main(){
    int i, t_entrada[NPROC], t_esperado[NPROC];
    processo processos[NPROC];

    //lendo tempo de entrada
    FILE *arquivo;
    arquivo = fopen("entrada.txt", "r");

    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        return 1; // Termina o programa com código de erro
    }

    //armazenando tempos de entrada
    if (fscanf(arquivo, "%d %d %d %d %d", &t_entrada[0], &t_entrada[1], &t_entrada[2], &t_entrada[3], &t_entrada[4]) != 5) {
        fprintf(stderr, "Erro ao ler os cinco números.\n");
        fclose(arquivo);
        return 1; // Termina o programa com código de erro
    }

    /*for(i=0; i<NPROC; i++){
        printf("proceeso %d: %d\n", i,t_entrada[i]);
    }*/

    //lendo tempos esperadso
    if (fscanf(arquivo, "%d %d %d %d %d", &t_esperado[0], &t_esperado[1], &t_esperado[2], &t_esperado[3], &t_esperado[4]) != 5) {
        fprintf(stderr, "Erro ao ler os cinco números.\n");
        fclose(arquivo);
        return 1; // Termina o programa com código de erro
    }

    /*for(i=0; i<NPROC; i++){
        printf("proceeso %d: %d\n", i,t_esperado[i]);
    }*/

    //criando processos
    for(i=0; i<NPROC; i++){
        processo p;
        p.PID = i;
        p.tempo_chegada = t_entrada[i];
        p.tempo_esperado = t_esperado[i];
        p.status = 0; //pronto
        processos[i] = p;
    }


    roundRobin(processos, 2);
    return 0;
}


void roundRobin(processo processos[], int quantum){
    int f; // guarda o tipo da fila
    int i, j; //iteradores
    int proc_conc = 0; //processos concluidos
    int ut=0;//unidades de tempo
    Fila alta, baixa;
    processo p, p_aux, proc_ord[NPROC], p_atual;

    //inicializando fila de processos
    inicializaFila(&alta);
    inicializaFila(&baixa);
 
    //copiando valores
    for(int i = 0; i < NPROC; i++){
        proc_ord[i] = processos[i];
    }

    //ordenando processos
    for(int i = 0; i < NPROC - 1; i++){
        for(int j = 0; j < NPROC - 1; j++){
            if(proc_ord[j].tempo_chegada > proc_ord[j+1].tempo_chegada){
                p_aux = proc_ord[j];
                proc_ord[j] = proc_ord[j+1];
                proc_ord[j+1] = p_aux;
                }
            }
    }

    //ate aqui tudo certo

    while(proc_conc != NPROC){
        printf("UT = %d\n", ut);
        //vendo se tem processo pronto
        for(i=0; i<NPROC; i++){
            if(proc_ord[i].tempo_chegada == ut){
                insereFila(&alta, proc_ord[i].PID);
                printf("processo %d entrou\n", proc_ord[i].PID);
            }
        }

        //se a fila de alta prioridade não estiver vazia
        if (!(filaVazia(&alta))){
            processos[peek(&alta)].tempo_esperado--; 
            p = processos[peek(&alta)];
            printf("Tempo restante do processo %d: %d\n", p.PID, p.tempo_esperado);
            f = 0;
        }
        //caso contrario, se a fila de baixa prioridade nao estiver vazia
        else if(!(filaVazia(&baixa))){
            processos[peek(&baixa)].tempo_esperado--;
            p = processos[peek(&baixa)];
            printf("Tempo restante do processo %d: %d\n", p.PID, p.tempo_esperado);
            f = 1;
        }
       
        
        if((ut % quantum == 0 && ut != 0) || p.tempo_esperado <= 0){
            if(f == 0){
                removeFila(&alta);
            }
            else if(f == 1){
                removeFila(&baixa);
            }

            if(p.tempo_esperado>0){
                printf("processo %d sofreu preempção e foi para a fila de baixa prioridade\n", p.PID);
                printf("tempo restante: %d\n", p.tempo_esperado);
                insereFila(&baixa, p.PID);
            }
            else{
                proc_conc++;
                printf("processo %d foi concluido\n", p.PID);
                processos[p.PID].status = 1;
            }           
        }
        ut++;


    }



}

//---------------------MANUTENÇÃO DE FILA--------------

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
        int removedItem = fila->itens[fila->inicio];
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