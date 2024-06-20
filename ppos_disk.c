#include "ppos_disk.h"

#include "ppos.h"
#include "disk.h"

disk_t disk_mgr;

void disk_mgr_Body() {
	while (1) {
		task_suspend(taskExec, NULL);
		task_yield();

		task_t *p = disk_mgr.diskQueue;

		if (p != NULL) {
			queue_remove((queue_t**) &disk_mgr.diskQueue, (queue_t*) p);
			task_resume(p);
		}
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

	return 0;
}

// leitura de um bloco, do disco para o buffer
int disk_block_read (int block, void *buffer) {
	int result = disk_cmd(DISK_CMD_STATUS, 0, 0);
	if (result != 1) {
		return -1;
	}

	if (disk_cmd(DISK_CMD_READ, block, buffer) < 0) {
		return -1;
	}

	task_suspend(taskExec, &disk_mgr.diskQueue);
	task_yield();

	return 0;
}

// escrita de um bloco, do buffer para o disco
int disk_block_write (int block, void *buffer) {
	int result = disk_cmd(DISK_CMD_STATUS, 0, 0);
	if (result != 1) {
		return -1;
	}

	if (disk_cmd(DISK_CMD_WRITE, block, buffer) < 0) {
		return -1;
	}

	task_suspend(taskExec, &disk_mgr.diskQueue);
	task_yield();

	return 0;
}
