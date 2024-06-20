projeto-b1: ppos_disk.c ppos_disk.h ppos-core-aux.c ppos_data.h disk.c
	gcc ppos_disk.c \
		ppos-core-aux.c disk.c libppos_static.a pingpong-disco1.c \
		-Wall -lrt -o projeto-b1


projeto-a: ppos-core-aux.c pingpong-scheduler-srtf.c ppos_data.h
	gcc ppos-core-aux.c pingpong-scheduler-srtf.c libppos_static.a -o projeto-a
