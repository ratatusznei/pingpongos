#include "ppos_disk.h"

#include "ppos.h"
#include "disk.h"

disk_t disk_mgr;

pedido_t *cscan(disk_queue_t *q) {
	pedido_t *next_pedido = NULL;
	pedido_t *p = q;

	// Verica os acima
	do {
		if (p->block >= disk_mgr.curr_block) {
			if (next_pedido == NULL) {
				next_pedido = p;
			}
			else if (p->block < next_pedido->block) {
				next_pedido = p;
			}
		}

		p = p->next;
	} while (p != q);

	// Acha o de menor block
	if (next_pedido == NULL) {
		disk_mgr.blocks_percorridos += abs(disk_mgr.numBlocks - disk_mgr.curr_block); // move para o final do disco
		disk_mgr.blocks_percorridos += disk_mgr.numBlocks; // move para o inicio
		disk_mgr.curr_block = 0;

		next_pedido = q;
		p = q->next;

		while (p != q) {
			if (next_pedido == NULL) {
				next_pedido = p;
			}
			else if (p->block < next_pedido->block) {
				next_pedido = p;
			}

			p = p->next;
		}
	}

	return next_pedido;
}

pedido_t *sstf(disk_queue_t *q) {
	int shortest_distance = abs(q->block - disk_mgr.curr_block);
	pedido_t *shortest_pedido = q;
	pedido_t *p = q->next;

	while (p != q) {
		int dist = abs(p->block - disk_mgr.curr_block);
		if (dist < shortest_distance) {
			shortest_distance = dist;
			shortest_pedido = p;
		}

		p = p->next;
	}

	return shortest_pedido;
}

pedido_t *fcfs(disk_queue_t *q) {
	return q;
}

void disk_mgr_Body() {
	while (countTasks > 1) {
		int result = disk_cmd(DISK_CMD_STATUS, 0, 0);
		if (result == DISK_STATUS_IDLE && disk_mgr.diskQueue != NULL) {
			// cscan   4223   26350 ms
			// sstf    4217   26466 ms
			// fcfs    7681   30025 ms
			mutex_lock(&disk_mgr.queue_mutex);
			pedido_t *p = fcfs(disk_mgr.diskQueue);
			mutex_unlock(&disk_mgr.queue_mutex);
	
			disk_cmd(p->type, p->block, p->buffer) ;

			task_suspend(taskExec, NULL);
			task_yield();

			disk_mgr.blocks_percorridos += abs(p->block - disk_mgr.curr_block);
			disk_mgr.curr_block = p->block;
			
			task_resume(p->task);
		}

		task_yield();
	}

	printf("DISC TASK EXIT\n    blocos percorridos: %d\n", disk_mgr.blocks_percorridos);
	task_exit(0);
}

// inicializacao do gerente de disco
// retorna -1 em erro ou 0 em sucesso
// numBlocks: tamanho do disco, em blocos
// blockSize: tamanho de cada bloco do disco, em bytes
int disk_mgr_init (int *numBlocks, int *blockSize) {
	if (disk_cmd(DISK_CMD_INIT, 0, 0) < 0) {
		return -1;
	}

	*numBlocks = disk_cmd(DISK_CMD_DISKSIZE, 0, 0);
	if (*numBlocks < 0) {
		return -1;
	}
	disk_mgr.numBlocks = *numBlocks;

	*blockSize = disk_cmd (DISK_CMD_BLOCKSIZE, 0, 0);
	if (*blockSize < 0) {
		return -1;
	}

	task_create(&disk_mgr.taskDisk, disk_mgr_Body, NULL);
	disk_mgr.taskDisk.user_task = 0;
	disk_mgr.taskDisk.remaining_time = 999999;

	disk_mgr.curr_block = 0;
	disk_mgr.blocks_percorridos = 0;

	mutex_create(&disk_mgr.queue_mutex);

	return 0;
}

int disk_block_command(int cmd, int block, void *buffer) {
	pedido_t *p = (pedido_t *) malloc(sizeof(pedido_t));
	p->next = NULL;
	p->prev = NULL;
	p->task = taskExec;
	p->type = cmd;
	p->block = block;
	p->buffer = buffer;

	mutex_lock(&disk_mgr.queue_mutex);
	queue_append((queue_t**) &disk_mgr.diskQueue, (queue_t*) p);
	mutex_unlock(&disk_mgr.queue_mutex);

	int result = disk_cmd(DISK_CMD_STATUS, 0, 0);
	if (result == DISK_STATUS_IDLE) {
		task_resume(&disk_mgr.taskDisk);
	}

	task_suspend(taskExec, NULL);
	task_yield();

	mutex_lock(&disk_mgr.queue_mutex);
	queue_remove((queue_t**) &disk_mgr.diskQueue, (queue_t*) p);
	mutex_unlock(&disk_mgr.queue_mutex);
	free(p);

	return 0;
}

// leitura de um bloco, do disco para o buffer
int disk_block_read (int block, void *buffer) {
	return disk_block_command(DISK_CMD_READ, block, buffer);
}

// escrita de um bloco, do buffer para o disco
int disk_block_write (int block, void *buffer) {
	return disk_block_command(DISK_CMD_WRITE, block, buffer);
}
