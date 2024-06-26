// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DINF UFPR
// Versão 1.2 -- Julho de 2017

// interface do gerente de disco rígido (block device driver)

#ifndef __DISK_MGR__
#define __DISK_MGR__

#include "ppos_data.h"
#include "ppos-core-globals.h"

// estruturas de dados e rotinas de inicializacao e acesso
// a um dispositivo de entrada/saida orientado a blocos,
// tipicamente um disco rigido.

// estrutura que representa um disco no sistema operacional

typedef struct disk_queue_t {
	struct disk_queue_t *next, *prev;
	struct task_t *task;
	int type; // READ / WRITE
	int block;
	void *buffer;
} disk_queue_t;

typedef struct disk_queue_t pedido_t;

typedef struct
{
	// completar com os campos necessarios
	int numBlocks;
	int curr_block;
	int blocks_percorridos;

	struct task_t taskDisk;

	mutex_t queue_mutex;
	struct disk_queue_t *diskQueue;
} disk_t ;

extern disk_t disk_mgr;

// inicializacao do gerente de disco
// retorna -1 em erro ou 0 em sucesso
// numBlocks: tamanho do disco, em blocos
// blockSize: tamanho de cada bloco do disco, em bytes
int disk_mgr_init (int *numBlocks, int *blockSize) ;

// leitura de um bloco, do disco para o buffer
int disk_block_read (int block, void *buffer) ;

// escrita de um bloco, do buffer para o disco
int disk_block_write (int block, void *buffer) ;

#endif
