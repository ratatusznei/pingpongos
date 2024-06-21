#include "ppos_disk.h"

#include "ppos.h"
#include "disk.h"

disk_t disk_mgr;

pedido_t *cscan(disk_queue_t *q) {
	return q;
}

pedido_t *sstf(disk_queue_t *q) {
	return q;
}

pedido_t *fcfs(disk_queue_t *q) {
	return q;
}

void disk_mgr_Body() {
	while (countTasks > 1) {
		int result = disk_cmd(DISK_CMD_STATUS, 0, 0);
		if (result == DISK_STATUS_IDLE && disk_mgr.diskQueue != NULL) {
			pedido_t *p = fcfs(disk_mgr.diskQueue);
	
			disk_cmd(p->type, p->block, p->buffer) ;

			task_suspend(taskExec, NULL);
			task_yield();

			task_resume(p->task);
		}

		task_yield();
	}
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

	*blockSize = disk_cmd (DISK_CMD_BLOCKSIZE, 0, 0);
	if (*blockSize < 0) {
		return -1;
	}

	task_create(&disk_mgr.taskDisk, disk_mgr_Body, NULL);
	disk_mgr.taskDisk.user_task = 0;
	disk_mgr.taskDisk.remaining_time = 999999;

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

	queue_append((queue_t**) &disk_mgr.diskQueue, (queue_t*) p);

	int result = disk_cmd(DISK_CMD_STATUS, 0, 0);
	if (result == DISK_STATUS_IDLE) {
		task_resume(&disk_mgr.taskDisk);
	}

	task_suspend(taskExec, NULL);
	task_yield();

	queue_remove((queue_t**) &disk_mgr.diskQueue, (queue_t*) p);
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
