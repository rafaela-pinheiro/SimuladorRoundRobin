/* versao do escalonador que considera o tempo de chegada 
com apenas 1 fila de processos */

#include <stdio.h>
#include <stdlib.h>
#define NPROC 5

// cores para printar no terminal
#define ANSI_COLOR_RED     "\x1b[31m" // vermelho
#define ANSI_COLOR_GREEN   "\x1b[32m" // verde
#define ANSI_COLOR_YELLOW  "\x1b[33m" // amarelo
#define ANSI_COLOR_BLUE    "\x1b[34m" // azul
#define ANSI_COLOR_MAGENTA "\x1b[35m" // magenta
#define ANSI_COLOR_CYAN    "\x1b[36m" // ciano
#define ANSI_COLOR_RESET   "\x1b[0m"  // reseta cor

//struct para guardar as informações das operações de IO
typedef struct{
    // tipos: 0 -> disco, 1 -> fita magnética, 2 -> impressora
    int tipo;
    int t_chegada;
} IO;

//struct para guardar as informações do processo
typedef struct {
    int PID;
    int tempo_chegada;
    int tempo_esperado;
    int status; //0 para pronto, 1 para terminado, 2 para bloqueado
    IO *listaIO;
    int qntdIO; // quantidade de operações de IO
} processo;

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
void roundRobinComIO(processo processos[], int quantum);

int main(){
    // auxiliares para os loops
    int i, j; 

    // vetores auxiliares
    int t_chegada[NPROC], t_esperado[NPROC], qntdIO[NPROC];

    // auxiliares para preenchimento da listaIO
    int aux1, aux2;

    int quantum = 2; // quantum do escalonador

    processo processos[NPROC]; // lista de processos

    printf(ANSI_COLOR_BLUE "\n== SIMULADOR DE ESCALONADOR USANDO ROUND ROBIN COM FEEDBACK ==\n\n" ANSI_COLOR_RESET);
    
    printf(ANSI_COLOR_GREEN "Lendo as informacoes do arquivo de entrada...\n" ANSI_COLOR_RESET);
    // lendo tempo de serviço de cada processo
    scanf("%d %d %d %d %d", &t_esperado[0], &t_esperado[1], &t_esperado[2], &t_esperado[3], &t_esperado[4]);
    printf("Os tempos de servico sao: %d %d %d %d %d\n", t_esperado[0], t_esperado[1], t_esperado[2], t_esperado[3], t_esperado[4]);

    // lendo tempo de chegada de cada processo
    scanf("%d %d %d %d %d", &t_chegada[0], &t_chegada[1], &t_chegada[2], &t_chegada[3], &t_chegada[4]);
    printf("Os tempos de chegada sao: %d %d %d %d %d\n", t_chegada[0], t_chegada[1], t_chegada[2], t_chegada[3], t_chegada[4]);

    // lendo qntd de operações de IO de cada processo
    scanf("%d %d %d %d %d", &qntdIO[0], &qntdIO[1], &qntdIO[2], &qntdIO[3], &qntdIO[4]);
    for (i = 0; i < NPROC; i++){
        printf("O processos %d tem %d operacao(oes) de IO\n", i, qntdIO[i]);
    }
    
    // criando processos
    printf(ANSI_COLOR_GREEN "Criando processos...\n" ANSI_COLOR_RESET);

    for(i = 0; i<NPROC; i++){
        processo p;
        p.PID = i;
        p.tempo_chegada = t_chegada[i];
        p.tempo_esperado = t_esperado[i];
        p.status = 0; //pronto
        p.qntdIO = qntdIO[i];
        if(p.qntdIO > 0){
            // se tiver IO, aloca a lista de IO do tamanho adequado
            IO *listaIOAux = malloc(p.qntdIO * sizeof(IO));
            if(!listaIOAux){
                printf("Erro ao alocar memoria\n");
                exit(1);
            }
            // lendo as operações de IO
            printf("Processo %d tem %d IO:\n", i, p.qntdIO);
            for (j = 0; j < p.qntdIO; j++){
                scanf("%d %d", &aux1, &aux2);
                printf("\tIO %d: tipo %d, tempo de chegada %d\n", j, aux1, aux2);
                listaIOAux[j].tipo = aux1;
                listaIOAux[j].t_chegada = aux2;
            }
            p.listaIO = listaIOAux;
            
        }
        else {
            printf("Processo %d nao tem IO\n", i);
        }
        processos[i] = p;
        printf("Processo %d alocado com sucesso!\n", i);
    }

    printf("Processos criados!\n");
    printf(ANSI_COLOR_GREEN "Fim da leitura das informacoes\n\n" ANSI_COLOR_RESET);
    
    // recapitulando as informações lidas
    for (i = 0; i < NPROC; i++){
        printf("Processo %d:\n\ttempo de servico %d\n\ttempo de chegada %d\n\tstatus %d\n", processos[i].PID, processos[i].tempo_esperado, processos[i].tempo_chegada, processos[i].status);
        if(processos[i].qntdIO > 0){
            printf("\tProcesso %d tem %d IO:\n", i, processos[i].qntdIO);
            for (j = 0; j < processos[i].qntdIO; j++){
                printf("\t\tIO %d: tipo %d, tempo de chegada %d\n", j, processos[i].listaIO[j].tipo, processos[i].listaIO[j].t_chegada);
            }
        }
        else {
            printf("\tProcesso %d nao tem IO\n", i);
        }
    }

    // simulando escalonamento
    scanf("%d", &quantum);
    printf(ANSI_COLOR_GREEN "Iniciando simulacao com quantum %d\n" ANSI_COLOR_RESET, quantum);
    roundRobinComIO(processos, quantum);

    return 0;
}

int mainAntiga(){
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

    //lendo tempos esperado
    if (fscanf(arquivo, "%d %d %d %d %d", &t_esperado[0], &t_esperado[1], &t_esperado[2], &t_esperado[3], &t_esperado[4]) != 5) {
        fprintf(stderr, "Erro ao ler os cinco números.\n");
        fclose(arquivo);
        return 1; // Termina o programa com código de erro
    }

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

void roundRobinComIO(processo processos[], int quantum){
    int f;              // guarda o tipo da fila
    int i, j;           // iteradores
    int proc_conc = 0;  // processos concluidos
    int ut=0;           // unidades de tempo
    Fila alta, baixa;
    Fila disco, fita, impressora;
    // processos
    processo p, p_aux;

    // processos ordenados por tempo de chegada
    processo proc_ord[NPROC];

    // inicializando filas de processos
    inicializaFila(&alta);
    inicializaFila(&baixa);

    // inicializando filas de IO
    inicializaFila(&disco);
    inicializaFila(&fita);
    inicializaFila(&impressora);
 
    // copiando valores
    for(int i = 0; i < NPROC; i++){
        proc_ord[i] = processos[i];
    }

    // ordenando processos
    for(int i = 0; i < NPROC - 1; i++){
        for(j = 0; j < NPROC - 1; j++){
            if(proc_ord[j].tempo_chegada > proc_ord[j+1].tempo_chegada){
                p_aux = proc_ord[j];
                proc_ord[j] = proc_ord[j+1];
                proc_ord[j+1] = p_aux;
                }
            }
    }

    // ate aqui tudo certo

    while(proc_conc != NPROC){
        /*
        Cada passo do loop representa uma unidade de tempo
        */
        printf("UT = %d\n", ut);
        // verifica se tem processo pronto
        for(i = 0; i < NPROC; i++){
            if(proc_ord[i].tempo_chegada == ut){
                insereFila(&alta, proc_ord[i].PID);
                printf(ANSI_COLOR_CYAN "Processo %d entrou na alta prioridade\n" ANSI_COLOR_RESET, proc_ord[i].PID);
            }
        }

        // se a fila de alta prioridade não estiver vazia, roda um processo dela
        if (!(filaVazia(&alta))){
            processos[peek(&alta)].tempo_esperado--; // decrementa tempo restante
            p = processos[peek(&alta)]; // 1o processo da fila de alta prioridade
            
            if (p.tempo_esperado == 0){
                printf("Processo %d foi concluido\n", p.PID);
            }
            
            printf("Tempo restante do processo %d: %d\n", p.PID, p.tempo_esperado);
            
            f = 0;
        }

        // caso contrario, se a fila de baixa prioridade nao estiver vazia, roda um processo dela
        else if(!(filaVazia(&baixa))){
            processos[peek(&baixa)].tempo_esperado--;
            p = processos[peek(&baixa)];
            printf("Tempo restante do processo %d: %d\n", p.PID, p.tempo_esperado);
            f = 1;
        }
       
        
        if((ut % quantum == 0 && ut != 0) || p.tempo_esperado <= 0){
            if(f == 0){ // se tem processo na fila de alta prioridade
                removeFila(&alta);
            }
            else if(f == 1){ // se tem processo na fila de baixa prioridade
                removeFila(&baixa);
            }

            if(p.tempo_esperado > 0){ // se o processo ainda precisa de tempo de CPU, vai pra fila de baixa prioridade
                printf(ANSI_COLOR_YELLOW "Processo %d sofreu preempção e foi para a fila de baixa prioridade\n" ANSI_COLOR_RESET, p.PID);
                printf("Tempo restante: %d\n", p.tempo_esperado);
                insereFila(&baixa, p.PID);
            }
            else{ // se o processo acabou, muda o status
                proc_conc++;
                printf(ANSI_COLOR_MAGENTA "Processo %d foi concluido\n" ANSI_COLOR_RESET, p.PID);
                processos[p.PID].status = 1;
            }           
        }
        ut++;
    }
}

void roundRobin(processo processos[], int quantum){
    int f; // guarda o tipo da fila
    int i, j; //iteradores
    int proc_conc = 0; //processos concluidos
    int ut=0;//unidades de tempo
    Fila alta, baixa;
    processo p, p_aux, proc_ord[NPROC]; // , p_atual;

    //inicializando fila de processos
    inicializaFila(&alta);
    inicializaFila(&baixa);
 
    //copiando valores
    for(int i = 0; i < NPROC; i++){
        proc_ord[i] = processos[i];
    }

    //ordenando processos
    for(int i = 0; i < NPROC - 1; i++){
        for(j = 0; j < NPROC - 1; j++){
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
                printf(ANSI_COLOR_CYAN "Processo %d entrou\n" ANSI_COLOR_RESET, proc_ord[i].PID);
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
                printf(ANSI_COLOR_YELLOW "Processo %d sofreu preempção e foi para a fila de baixa prioridade\n" ANSI_COLOR_RESET, p.PID);
                printf("Tempo restante: %d\n", p.tempo_esperado);
                insereFila(&baixa, p.PID);
            }
            else{
                proc_conc++;
                printf(ANSI_COLOR_MAGENTA "Processo %d foi concluido\n" ANSI_COLOR_RESET, p.PID);
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
        // int removedItem = fila->itens[fila->inicio];
        //printf("Elemento %d removido da fila\n", removedItem);

        // printf("Elemento %d removido da fila\n", fila->itens[fila->inicio]);
        
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