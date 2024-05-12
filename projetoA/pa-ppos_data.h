/*
 * Equipe:
 * Fernando Henrique Ratusznei Caetano
 * Gustavo Esmanhotto Bareta
 */

// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DINF UFPR
// Versão 1.1 -- Julho de 2016

// Estruturas de dados internas do sistema operacional

#ifndef __PPOS_DATA__
#define __PPOS_DATA__

#include <stdio.h>
#include <ucontext.h>		// biblioteca POSIX de trocas de contexto
#include "queue.h"		// biblioteca de filas genéricas

// Estrutura que define um Task Control Block (TCB)
typedef struct task_t
{
   struct task_t *prev, *next ;		// ponteiros para usar em filas
   int id ;				// identificador da tarefa
   ucontext_t context ;			// contexto armazenado da tarefa
   unsigned char state;  // indica o estado de uma tarefa (ver defines no final do arquivo ppos.h): 
                          // n - nova, r - pronta, x - executando, s - suspensa, e - terminada
   struct task_t* queue;
   struct task_t* joinQueue;
   int exitCode;
   unsigned int awakeTime; // used to store the time when it should be waked up

   // ... (outros campos deve ser adicionados APOS esse comentario)
   int running_time;  // tempo de execução
   int execution_time; // tempo de execução estimado
   int remaining_time; // tempo restante estimado  
   int quantum_counter; // contador de quantum
   int user_task; // flag para marcar tasks de sistema
   int activations; // contador de ativacoes para imprimir métricas
   int create_time; // tempo de criacao da task em systicks
} task_t ;

// estrutura que define um semáforo
typedef struct {
    struct task_t *queue;
    int value;

    unsigned char active;
} semaphore_t ;

// estrutura que define um mutex
typedef struct {
    struct task_t *queue;
    unsigned char value;

    unsigned char active;
} mutex_t ;

// estrutura que define uma barreira
typedef struct {
    struct task_t *queue;
    int maxTasks;
    int countTasks;
    unsigned char active;
    mutex_t mutex;
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct {
    void* content;
    int messageSize;
    int maxMessages;
    int countMessages;
    
    semaphore_t sBuffer;
    semaphore_t sItem;
    semaphore_t sVaga;
    
    unsigned char active;
} mqueue_t ;

#endif

