#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NPROC 5
#define IO_total 3

// Estrutura para representar uma operação de I/O
typedef struct
{
    int tipo;          // 0 -> disco (3 u.t.), 1 -> fita (5 u.t.), 2 -> impressora (7 u.t.), 3 -> concluído
    int tempo;         // tempo de chegada em relação à execução do processo
    int tempo_chegada; // tempo de chegada do IO
    int duracao;       // duração do IO dependendo do tipo
} IO;

// struct para guardar as informações do processo
typedef struct
{
    int PID;
    int tempo_esperado;
    int tempo_rodado;  // quanto tempo o processo já esteve em execução
    int tempo_chegada; // tempo de chegada do processo
    // int prioridade; //talvez nao precise
    int status;   // 0-> pronto, 1 -> bloqueado, 2 -> concluído, 3 -> executando
    IO *listaIO;  // Lista de operações de I/O
    int num_io;   // Número de operações de I/O
    int io_lidos; // Número de operações de I/O já lidas
} processo;

// guarda informações da fila
typedef struct
{
    int itens[NPROC];
    int inicio;
    int fim;
} Fila;

void inicializaFila(Fila *fila);
int filaVazia(Fila *fila);
int filaCheia(Fila *fila);
void insereFila(Fila *fila, int valor);
void removeFila(Fila *fila);
void printFila(Fila *fila);
int peek(Fila *fila);
void roundRobin(processo processos[], int quantum);
void insereFilaIO(Fila *filaD, Fila *filaF, Fila *filaI, processo p, int momento_atual);

void insereFilaIO(Fila *filaD, Fila *filaF, Fila *filaI, processo p, int momento_atual)
{
    // insere o processo p na fila de IO correspondente
    switch (p.listaIO[0].tipo)
    {
    case 0: // disco
        insereFila(filaD, p.PID);
        p.listaIO[0].duracao = 3;
        printf("Processo %d foi para IO de disco\n", p.PID);
        break;
    case 1: // fita
        insereFila(filaF, p.PID);
        p.listaIO[0].duracao = 5;
        printf("Processo %d foi para IO de fita magnetica\n", p.PID);
        break;
    case 2: // impressora
        insereFila(filaI, p.PID);
        p.listaIO[0].duracao = 7;
        printf("Processo %d foi para IO de impressora\n", p.PID);
        break;
    default:
        printf("Erro ao inserir processo %d na fila de IO\n", p.PID);
        break;
    }
    p.listaIO[0].tipo = 3;                      // remove o IO que acabou de ser direcionado para sua fila, marca como concluído
    p.io_lidos++;                               // atualiza o número de IO já lidos
    p.listaIO[0].tempo_chegada = momento_atual; // atualiza o tempo de chegada do IO
}

void roundRobin(processo processos[], int quantum)
{
    int tempo_total = 0, k = 0;
    int processos_concluidos = 0;

    Fila altaPrioridade;
    Fila baixaPrioridade;
    Fila impressao;
    Fila disco;
    Fila fita;

    // inicializando filas
    inicializaFila(&altaPrioridade);
    inicializaFila(&baixaPrioridade);
    inicializaFila(&impressao);
    inicializaFila(&disco);
    inicializaFila(&fita);

    // inserindo processos novos na fila de alta prioridade
    for (k = 0; k < NPROC; k++)
    {
        insereFila(&altaPrioridade, k);
    }
    printf("TEMPO DECORRIDO: 0\n");
    int p;
    int m;
    while (processos_concluidos <= NPROC)
    {
        processos_concluidos = 0;
        // se a fila de alta prioridade não estiver vazia
        if (!(filaVazia(&altaPrioridade)))
        {
            p = peek(&altaPrioridade);
            processo procP = processos[p];
            printf("num io do processo %d: %d\n", procP.PID, procP.num_io);
            if (procP.io_lidos != procP.num_io)
            {
                printf("Processo %d tem IO\n", procP.PID);
                // tem IO a ser feito
                for (m = 0; m < procP.num_io; m++)
                {

                    printf("m = %d\n", m);

                    if (procP.listaIO[m].tipo == 3)
                    {
                        // IO já foi feito
                        printf("IO %d do processo %d ja foi feito\n", m, procP.PID);
                        // m++;
                        continue;
                    }

                    printf("\t IO %d do processo %d:\n", m, procP.PID);
                    if (procP.listaIO[m].tempo > (procP.tempo_rodado + quantum))
                    { // IO vai entrar depois da execução
                        // processo vai executar durante quantum
                        procP.tempo_esperado = procP.tempo_esperado - quantum;
                        procP.tempo_rodado += quantum;
                        removeFila(&altaPrioridade);

                        // se nao precisa de mais processamento, fica como concluido
                        if (procP.tempo_esperado <= 0)
                        {
                            if (procP.tempo_esperado < 0)
                            {
                                // ajustando contagem de tempo
                                tempo_total = tempo_total + procP.tempo_esperado;
                            }
                            procP.status = 2; // mudando estado para concluido
                            // processos_concluidos++;
                            printf("Processo %d terminou execucao\n", processos[p].PID);
                        }

                        // se ainda precisa de processamento, manda pra fila de baixa prioridade
                        else
                        {
                            printf("Processo %d foi para baixa prioridade\n", processos[p].PID);
                            printf("Tempo restante: %d\n", processos[p].tempo_esperado);
                            insereFila(&baixaPrioridade, p);
                        }
                    }
                    if (procP.listaIO[m].tempo < (procP.tempo_rodado + quantum))
                    { // IO vai entrar durante a execução
                        // processo vai executar durante procP.listaIO[i].tempo - procP.tempo_rodado u.t.
                        int tempo_exec = procP.listaIO[m].tempo - procP.tempo_rodado;
                        procP.tempo_esperado = procP.tempo_esperado - tempo_exec;

                        // remove da fila atual
                        // removeFila(&altaPrioridade);

                        // insere na fila de IO correspondente
                        insereFilaIO(&disco, &fita, &impressao, procP, tempo_total + tempo_exec);
                        procP.status = 1; // mudando estado para bloqueado

                        // insereFila(&baixaPrioridade, p);
                    }
                    else if (procP.listaIO[m].tempo == (procP.tempo_rodado + quantum))
                    { // pode fazer IO ao final dessa execução
                        // processo vai executar durante quantum
                        procP.tempo_esperado = procP.tempo_esperado - quantum;
                        procP.tempo_rodado += quantum;
                        // removeFila(&altaPrioridade);

                        if (procP.tempo_esperado < 0)
                        {
                            // ajustando contagem de tempo
                            tempo_total = tempo_total + procP.tempo_esperado;
                        }

                        // printf("Processo %d terminou execucao\n", processos[p].PID);

                        // trata IO
                        procP.status = 1; // bloqueado
                        // insere na fila de IO correspondente
                        insereFilaIO(&disco, &fita, &impressao, procP, tempo_total + quantum);

                        // insereFila(&baixaPrioridade, p);
                    }
                }
            }
            else
            { // não faz IO
                // processo mais prioritario consumindo sua fatia de tempo
                processos[p].tempo_esperado = processos[p].tempo_esperado - quantum;
                removeFila(&altaPrioridade);

                // se nao precisa de mais processamento, fica como concluido
                if (processos[p].tempo_esperado <= 0)
                {
                    processos[p].status = 2; // mudando estado para concluido
                    // processos_concluidos++;
                    printf("Processo %d terminou\n", processos[p].PID);

                    if (processos[p].tempo_esperado < 0)
                    {
                        // ajustando contagem de tempo
                        tempo_total = tempo_total + processos[p].tempo_esperado;
                    }
                }

                // se ainda tiver tempo, manda pra fila de baixa prioridade
                else
                {
                    printf("Processo %d foi para baixa prioridade\n", processos[p].PID);
                    printf("Tempo restante: %d\n", processos[p].tempo_esperado);
                    insereFila(&baixaPrioridade, p);
                }
            }
        }

        // se a fila de alta prioridade estiver vazia;
        else if (!(filaVazia(&baixaPrioridade)))
        {
            p = peek(&baixaPrioridade);
            processos[p].tempo_esperado = processos[p].tempo_esperado - quantum;
            removeFila(&baixaPrioridade);

            // se o processo não tiver mais demanda
            if (processos[p].tempo_esperado <= 0)
            {
                processos[p].status = 2; // atualiza estado para concluido
                // processos_concluidos++;
                printf("Processo %d terminou\n", processos[p].PID);

                if (processos[p].tempo_esperado < 0)
                {
                    // ajustando contagem de tempo
                    tempo_total = tempo_total + processos[p].tempo_esperado;
                }
            }
            else
            {
                printf("Processo %d sofreu preempção\n", processos[p].PID);
                printf("Tempo restante: %d\n", processos[p].tempo_esperado);
                insereFila(&baixaPrioridade, p);
            }
        }

        /*printf("Alta prioridade: ");
        printFila(&altaPrioridade);
        printf("Baixa prioridade: ");
        printFila(&baixaPrioridade);*/

        // verificar se pode tirar algum processo da fila de IO
        if (!(filaVazia(&disco)))
        {
            p = peek(&disco);
            processo procP = processos[p];

            if ((procP.listaIO[0].tempo_chegada + procP.listaIO[0].tempo) >= tempo_total)
            {
                // processo pode sair da fila de IO
                removeFila(&disco);
                insereFila(&baixaPrioridade, p);
                procP.status = 0; // pronto
                printf("Processo %d saiu da fila de IO de disco\n", procP.PID);
            }
        }
        if (!(filaVazia(&fita)))
        {
            p = peek(&fita);
            processo procP = processos[p];
            if ((procP.listaIO[0].tempo_chegada + procP.listaIO[0].tempo) >= tempo_total)
            {
                // processo pode sair da fila de IO
                removeFila(&fita);
                insereFila(&altaPrioridade, p);
                procP.status = 0; // pronto
                printf("Processo %d saiu da fila de IO de fita\n", procP.PID);
            }
        }
        if (!(filaVazia(&impressao)))
        {
            p = peek(&impressao);
            processo procP = processos[p];
            if ((procP.listaIO[0].tempo_chegada + procP.listaIO[0].tempo) >= tempo_total)
            {
                // processo pode sair da fila de IO
                removeFila(&impressao);
                insereFila(&altaPrioridade, p);
                procP.status = 0; // pronto
                printf("Processo %d saiu da fila de IO de impressao\n", procP.PID);
            }
        }

        for (int j = 0; j < NPROC; j++)
        {
            if (processos[j].status == 2)
            {
                processos_concluidos++;
            }
        }

        // imprime os processos na fila de baixa prioridade
        printf("Processos na fila de baixa prioridade: \n");
        printFila(&baixaPrioridade);

        // imprime os processos na fila de alta prioridade
        printf("Processos na fila de alta prioridade: \n");
        printFila(&altaPrioridade);

        printf("Processos concluidos: %d\n", processos_concluidos);
        tempo_total += quantum;
        printf("\nTEMPO DECORRIDO: %d\n", tempo_total);
    }

    printf("\nTEMPO DECORRIDO TOTAL: %d\n", tempo_total);
}

/*-------------- Manutenção de Fila ------------------*/

void inicializaFila(Fila *fila)
{
    fila->inicio = -1;
    fila->fim = -1;
}

int filaVazia(Fila *fila)
{
    return (fila->inicio == -1 && fila->fim == -1);
}

int filaCheia(Fila *fila)
{
    return (fila->fim + 1) % NPROC == fila->inicio;
}

void insereFila(Fila *fila, int valor)
{
    if (filaCheia(fila))
    {
        printf("Ops, fila cheia\n");
        return;
    }
    else
    {
        if (filaVazia(fila))
        {
            fila->inicio = 0;
        }
        fila->fim = (fila->fim + 1) % NPROC;
        fila->itens[fila->fim] = valor;
        // printf("Elemento %d inserido na fila.\n", valor);
    }
}

void removeFila(Fila *fila)
{
    if (filaVazia(fila))
    {
        printf("Ops, fila vazia\n");
        return;
    }
    else
    {
        int removedItem = fila->itens[fila->inicio];
        printf("Elemento %d removido da fila\n", removedItem);
        if (fila->inicio == fila->fim)
        {
            // Se só tiver um elemento na fila
            inicializaFila(fila);
        }
        else
        {
            fila->inicio = (fila->inicio + 1) % NPROC;
        }
    }
}

void printFila(Fila *fila)
{
    printf("Valores da fila: ");
    int i = fila->inicio;

    do
    {
        printf("%d ", fila->itens[i]);
        i = (i + 1) % NPROC;
    } while (i != (fila->fim + 1) % NPROC);

    printf("\n");
}

int peek(Fila *fila)
{
    if (filaVazia(fila))
    {
        printf("Ops, fila vazia. Não há primeiro item.\n");
        return -1; // Ou algum valor que indique que a fila está vazia
    }
    else
    {
        return fila->itens[fila->inicio];
    }
}

int main(int argc, char *argv[])
{
    int i;

    // tempo esperado de cada um dos processos
    int tempos[NPROC]; // = {4, 2, 8, 9, 6};

    // Lendo arquivo de entrada
    if (argc < 2)
    {
        printf("Digite: %s <nome do arquivo de entrada>.txt\n", argv[0]);
        return 0;
    }

    char nomeArquivo[100];
    strcpy(nomeArquivo, argv[1]);
    FILE *arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo. Encerrando o programa...\n");
        return 0;
    }
    // printf("Arquivo aberto com sucesso!\n");

    char virgula;
    for (int j = 0; j < NPROC; j++)
    {
        if (fscanf(arquivo, "%d%c", &tempos[j], &virgula) == 2)
        {
            // printf("%d\t", tempos[j]);
        }
    }
    // printf("\n");
    int IO_list[IO_total][3]; //= {{0, 1, 3}, {1, 2, 1}, {2, 1, 5}}; // p.PID, tipo, tempo de chegada

    for (int a = 0; a < 3; a++)
    {
        for (int j = 0; j < IO_total; j++)
        {
            if (fscanf(arquivo, "%d%c", &IO_list[j][a], &virgula) == 2)
            {
                // printf("%d\t", IO_list[j][a]);
            }
        }
    }
    // printf("\n");
    fclose(arquivo);

    // criando lista de processos;
    processo processos[NPROC];

    // criando os processos e guardando no array de processos
    for (i = 0; i < NPROC; i++)
    {
        processo p;
        p.PID = i;
        p.tempo_esperado = tempos[i];
        p.status = 0; // pronto
        p.num_io = 0;
        p.io_lidos = 0;
        p.listaIO = NULL;
        p.tempo_rodado = 0;
        for (int j = 0; j < IO_total; j++)
        {
            if (IO_list[j][0] == i)
            {               // é IO desse processo
                p.num_io++; // inicializa o número de operações de I/O
                p.listaIO = realloc(p.listaIO, p.num_io * sizeof(IO));
                p.listaIO[p.num_io - 1].tipo = IO_list[j][1];  // Configura o tipo de I/O
                p.listaIO[p.num_io - 1].tempo = IO_list[j][2]; // Configura o tempo de chegada
                                                               // printf("O processo %d tem %d IO\n", p.PID, p.num_io);
                                                               // printf("O processo %d tem IO do tipo %d\n", p.PID, p.listaIO[p.num_io - 1].tipo);
            }
        }

        processos[i] = p;
    }

    roundRobin(processos, 2);

    return 0;
}
